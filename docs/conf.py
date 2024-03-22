import os
import subprocess

from sphinx.application import Sphinx
from pathlib import Path

project = 'Catch2'
copyright = '2023, Martin Hořeňovský'
author = 'Martin Hořeňovský'

extensions = [
    "myst_parser",
    "sphinx_design",
    "sphinx_togglebutton",
    "breathe",
]

templates_path = []
exclude_patterns = [
    'build',
    '_build',
    'Thumbs.db',
    '.DS_Store',
    "Readme.md",
]
source_suffix = [".md"]

html_theme = 'furo'
# html_static_path = ['_static']

myst_enable_extensions = [
    "tasklist",
    "colon_fence",
]

breathe_projects = {
    "Catch2": "build/doxygen/xml",
}
breathe_default_project = "Catch2"


def generate_doxygen_xml(app: Sphinx):
    """
    Run the doxygen commands
    """
    os.chdir(Path(app.confdir).parent)
    subprocess.run(["doxygen"])


def setup(app: Sphinx):
    # Add hook for building doxygen xml when needed
    app.connect("builder-inited", generate_doxygen_xml)
