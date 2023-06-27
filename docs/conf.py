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
#html_static_path = ['_static']

myst_enable_extensions = [
    "tasklist",
    "colon_fence",
]
