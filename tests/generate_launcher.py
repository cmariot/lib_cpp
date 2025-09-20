#!/usr/bin/env python3
import os
import re
from pathlib import Path

ROOT = Path(__file__).parent

TESTS_DIR = ROOT

CPP_PATTERN = re.compile(
    r'extern\s+"C"\s+int\s+([A-Za-z0-9_]+)\s*\(\s*void\s*\)'
)

ALT_PATTERN = re.compile(
    r'\bint\s+([A-Za-z0-9_]+)\s*\(\s*void\s*\)'
)


def pascal_case(s: str) -> str:
    parts = s.split('_')
    return ''.join(p.capitalize() for p in parts if p)


def find_tests():
    tests = []  # list of (func, dir)
    for dirpath, dirnames, filenames in os.walk(TESTS_DIR):
        # skip framework directory and hidden
        if 'framework' in Path(dirpath).parts:
            continue
        for fname in filenames:
            if not fname.endswith('.cpp'):
                continue
            if fname == 'tests_launcher.cpp':
                continue
            fpath = Path(dirpath) / fname
            text = fpath.read_text(encoding='utf-8')
            for m in CPP_PATTERN.finditer(text):
                tests.append((m.group(1), Path(dirpath).name))
            # fallback: find functions named *_test
            if not any(CPP_PATTERN.search(text) for _ in [0]):
                for m in ALT_PATTERN.finditer(text):
                    name = m.group(1)
                    if name.endswith('_test'):
                        tests.append((name, Path(dirpath).name))
    return tests


def generate(tests):
    out = []
    out.append('#include "framework/includes/libunit.hpp"')
    out.append('#include "test_utils.hpp"')
    out.append('')
    # extern declarations
    funcs = [t[0] for t in tests]
    for f in funcs:
        out.append(f'extern "C" int {f}(void);')
    out.append('')
    out.append('int main() {')
    out.append('    t_test *tests = NULL;')
    out.append('')
    for func, dirn in tests:
        suite = pascal_case(dirn)
        testname = func
        if testname.endswith('_test'):
            testname = testname[:-5]
        out.append(
            '    load_test' +
            f'(&tests, "{suite}", "{testname}", (void*){func}, 0);'
        )
    out.append('')
    out.append('    return launch_tests(&tests);')
    out.append('}')
    (ROOT / 'tests_launcher.cpp').write_text('\n'.join(out), encoding='utf-8')


if __name__ == '__main__':
    tests = find_tests()
    if not tests:
        print('No tests found. Generated launcher will be empty.')
    generate(tests)
    print(
        'Generated tests/tests_launcher.cpp with {} tests.'.format(len(tests))
    )
