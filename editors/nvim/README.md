# Neovim support

Instructions for adding tree sitter support for Gren in [Neovim](https://neovim.io/) how to interpret Gren code highlighting using the [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) package

# Installing

This guide assumes that you already have [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) installed and setup.

There are two steps to setting up the Gren Treesitter support in Neovim.

1. Adding .gren file recognition support to neovim
2. Adding the Gren Treesitter Parser to the list of supported Treesitter Parsers
3. Adding the the Gren Treesitter query files to Neovims runtime path

## Step 1 - Adding Gren filetype recognition support

Simply add this line to your config - this will cause neovim to check the file extension, and when
it matches with .gren, it will automaticall set the filetype to 'gren'
```lua
vim.cmd.autocmd "BufNewFile,BufRead *.gren setfiletype gren"
```

## Step 2 - Adding the parser to the list of supported parsers

Add these lines to your config after [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) has been loaded
```lua
local ts = require "nvim-treesitter.configs"
local parsers = require "nvim-treesitter.parsers"

parsers.get_parser_configs().gren = {
    filetype = "gren",
    install_info = {
      branch = "main",
      files = { "src/parser.c", "src/scanner.c" },
      url = "https://github.com/MaeBrooks/tree-sitter-gren",
    }
}

ts.setup {
    highlight = {
      enable = true
    }
}
```

## Step 3 - Adding the query files to Neovims runtime path

> In order for [nvim-treesitter](https://github.com/nvim-treesitter/nvim-treesitter) and Neovim to
understand parse results, It needs to understand the generated query results. Adding the files under
`editor/nvim/gren` to Neovims [runtime path](https://neovim.io/doc/user/options.html#'runtimepath')
will allow it do so.

Currently, You will need to clone this repository and run these commands from the the repository
root.

**These commands create a symlink, if you do not want to keep these files around, copy the folders
instead**

### For Vanilla Neovim

```bash
mkdir -p ~/.config/nvim/queries
ln -s editors/nvim/gren ~/.config/nvim/queries/gren
```

### For the Lazy package manager

If using as through [Lazy](https://github.com/folke/lazy.nvim)
```bash
ln -s editors/nvim/gren ~/.local/share/nvim/lazy/nvim-treesitter/queries/gren
```
