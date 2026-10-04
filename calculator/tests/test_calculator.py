"""Run with: python3 -m unittest discover -s calculator/tests -v (repository root)."""
import math
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


class HistoryRegressionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build_directory = tempfile.TemporaryDirectory()
        cls.binary = Path(cls.build_directory.name) / 'Calculator'
        source = Path(__file__).resolve().parents[1] / 'main.c'
        subprocess.run([os.environ.get('CC', 'cc'), '-std=c17', '-Wall', '-Wextra',
                        '-Wpedantic', '-Werror', str(source), '-lm', '-o',
                        str(cls.binary)], check=True, capture_output=True, text=True)

    @classmethod
    def tearDownClass(cls):
        cls.build_directory.cleanup()

    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.history = Path(self.directory.name) / 'calculator_history.txt'

    def run_calculator(self, *arguments, text=''):
        return subprocess.run([str(self.binary), *arguments], cwd=self.directory.name,
                              input=text, text=True, capture_output=True, timeout=5)

    def test_subnormal_calculation_survives_restart(self):
        result = self.run_calculator('1e-300 * 1e-20')
        self.assertEqual(result.returncode, 0, result.stderr)
        saved = float(self.history.read_text().split('\t')[0])
        self.assertTrue(math.isfinite(saved))
        self.assertGreater(saved, 0)
        restarted = self.run_calculator(text='2\n4\n')
        self.assertEqual(restarted.returncode, 0, restarted.stderr)
        self.assertIn('1e-300 * 1e-20 =', restarted.stdout)
        loaded = float(restarted.stdout.split('1e-300 * 1e-20 = ')[1].splitlines()[0])
        self.assertEqual(loaded, saved)

    def test_history_accepts_signed_subnormals_and_zero(self):
        self.history.write_text('1e-320\ttiny positive\n-1e-320\ttiny negative\n0\tzero\n')
        result = self.run_calculator(text='2\n4\n')
        self.assertIn('tiny positive =', result.stdout)
        self.assertIn('tiny negative =', result.stdout)
        self.assertIn('zero = 0', result.stdout)

    def test_history_rejects_invalid_overflow_and_underflow_to_zero(self):
        self.history.write_text('1e999\toverflow\n1e-999\tunderflow\nnan\tnot a number\n'
                                'inf\tinfinity\ninvalid\tbad number\n5\tvalid\n')
        result = self.run_calculator(text='2\n4\n')
        self.assertIn('1. valid = 5', result.stdout)
        for expression in ['overflow', 'underflow', 'not a number', 'infinity', 'bad number']:
            self.assertNotIn(expression + ' =', result.stdout)

    def test_cli_fails_when_history_cannot_be_saved(self):
        self.history.mkdir()
        result = self.run_calculator('10 + 5')
        self.assertEqual(result.returncode, 1)
        self.assertIn('Result: 15', result.stdout)
        self.assertIn('Cannot save history', result.stderr)

    def test_cli_success_saves_history(self):
        result = self.run_calculator('10 + 5')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(self.history.read_text(), '15\t10 + 5\n')

    def test_interactive_session_keeps_result_after_save_failure(self):
        self.history.mkdir()
        result = self.run_calculator(text='1\n10 + 5\nM+\nMR\n2\n4\n')
        self.assertEqual(result.returncode, 0)
        self.assertIn('Memory: 15', result.stdout)
        self.assertIn('10 + 5 = 15', result.stdout)
        self.assertIn('Cannot save history', result.stderr)


if __name__ == '__main__':
    unittest.main()
