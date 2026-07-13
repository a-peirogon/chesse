{
  description = "Chess engine in C++17";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs   = nixpkgs.legacyPackages.${system};
  in {
    packages.${system} = {
      # CLI interactivo: nix run .#chess
      chess = pkgs.stdenv.mkDerivation {
        name = "chess";
        src  = ./.;
        nativeBuildInputs = [ pkgs.gcc pkgs.cmake ];
        configurePhase = "cmake -S . -B build";
        buildPhase     = "cmake --build build --target chess";
        installPhase   = ''
          mkdir -p $out/bin
          cp build/chess $out/bin/
        '';
      };

      # Motor UCI para SCID vs PC: nix run .#chess_uci
      chess_uci = pkgs.stdenv.mkDerivation {
        name = "chess_uci";
        src  = ./.;
        nativeBuildInputs = [ pkgs.gcc pkgs.cmake ];
        configurePhase = "cmake -S . -B build";
        buildPhase     = "cmake --build build --target chess_uci";
        installPhase   = ''
          mkdir -p $out/bin
          cp build/chess_uci $out/bin/
        '';
      };

      default = self.packages.${system}.chess_uci;
    };

    # nix develop  →  shell con gcc, cmake y gdb
    devShells.${system}.default = pkgs.mkShell {
      packages = [ pkgs.gcc pkgs.cmake pkgs.gdb ];
      shellHook = ''
        echo "Chess engine dev shell"
        echo "  cmake -S . -B build && cmake --build build"
      '';
    };
  };
}
