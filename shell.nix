{ pkgs ? import <nixpkgs> { }, ... }:
pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.ccls
  ];
  buildInputs = [
    pkgs.clang-analyzer
    pkgs.clang-tools
    pkgs.cgreen
  ];
}
