{ pkgs ? import <nixpkgs> { }, ... }:
pkgs.mkShell {
  buildInputs = [
    pkgs.clang-analyzer
    pkgs.clang-tools
    pkgs.cgreen
    pkgs.gnumake
  ];
}
