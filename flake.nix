{
  description = "tree-sitter-elm";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in {
        devShell = pkgs.mkShell {
          buildInputs = [
            pkgs.tree-sitter
            pkgs.nodejs
            pkgs.python
            pkgs.cargo
            pkgs.elmPackages.elm
            pkgs.rustc
            pkgs.rustfmt
            pkgs.clippy
          ];
          RUST_SRC_PATH =
            "${pkgs.rust.packages.stable.rustPlatform.rustLibSrc}";
        };
      });
}
