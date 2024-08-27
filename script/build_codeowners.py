#!/usr/bin/env python3
import argparse
from collections import defaultdict
from pathlib import Path
import sys

from esphome.config import get_component, get_platform
from esphome.const import KEY_CORE, KEY_TARGET_FRAMEWORK
from esphome.core import CORE
from esphome.helpers import write_file_if_changed

parser = argparse.ArgumentParser()
parser.add_argument(
    "--check", help="Check if the CODEOWNERS file is up to date.", action="store_true"
)
args = parser.parse_args()

# The root directory of the repo
root = Path(__file__).parent.parent
components_dir = root / "esphome" / "components"

BASE = """
# This file is generated by script/build_codeowners.py
# People marked here will be automatically requested for a review
# when the code that they own is touched.
#
# Every time an issue is created with a label corresponding to an integration,
# the integration's code owner is automatically notified.

# Core Code
pyproject.toml @esphome/core
esphome/*.py @esphome/core
esphome/core/* @esphome/core

# Integrations
""".strip()

parts = [BASE]

# Fake some directory so that get_component works
CORE.config_path = str(root)
CORE.data[KEY_CORE] = {KEY_TARGET_FRAMEWORK: None}

codeowners = defaultdict(list)

for path in components_dir.iterdir():
    if not path.is_dir():
        continue
    if not (path / "__init__.py").is_file():
        continue

    name = path.name
    comp = get_component(name)
    if comp is None:
        print(
            f"Cannot find component {name}. Make sure current path is pip installed ESPHome"
        )
        # sys.exit(1)

    if name == "ballu_old":
        codeowners[f"esphome/components/{name}/*"].extend(["@Midnighter32"])
    elif comp is None:
        codeowners[f"esphome/components/{name}/*"].extend(["@dwmw2"])
    else:
        codeowners[f"esphome/components/{name}/*"].extend(comp.codeowners)

    for platform_path in path.iterdir():
        platform_name = platform_path.stem
        platform = get_platform(platform_name, name)
        if platform is None:
            continue

        if platform_path.is_dir():
            # Sub foldered platforms get their own line
            if not (platform_path / "__init__.py").is_file():
                continue
            codeowners[f"esphome/components/{name}/{platform_name}/*"].extend(
                platform.codeowners
            )
            continue

        # Non-subfoldered platforms add to codeowners at component level
        if not platform_path.is_file() or platform_path.name == "__init__.py":
            continue
        codeowners[f"esphome/components/{name}/*"].extend(platform.codeowners)


for path, owners in sorted(codeowners.items()):
    owners = sorted(set(owners))
    if not owners:
        continue
    for owner in owners:
        if not owner.startswith("@"):
            print(
                f"Codeowner {owner} for integration {path} must start with an '@' symbol!"
            )
            sys.exit(1)
    parts.append(f"{path} {' '.join(owners)}")


# End newline
parts.append("")
content = "\n".join(parts)
codeowners_file = root / "CODEOWNERS"

if args.check:
    if codeowners_file.read_text() != content:
        print("CODEOWNERS file is not up to date.")
        print("Please run `script/build_codeowners.py`")
        sys.exit(1)
    print("CODEOWNERS file is up to date")
else:
    write_file_if_changed(codeowners_file, content)
    print("Wrote CODEOWNERS")
