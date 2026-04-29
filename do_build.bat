@echo off
cd /d C:\Users\ali.erdogan\Documents\TD_Test
C:\msys64\mingw64\bin\gcc.exe -std=c99 -Wall -O2 -pipe -IC:/msys64/mingw64/include src/audio.c src/dungeon.c src/enemy.c src/guardian.c src/homecity.c src/hud.c src/i18n.c src/input.c src/logger.c src/main.c src/manager.c src/map.c src/particle.c src/projectile.c src/rarity.c src/save.c src/settings.c src/siege.c src/tower.c src/ui.c src/util.c src/wave.c -LC:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o release/game.exe >build_tmp\out.txt 2>&1
echo ExitCode=%ERRORLEVEL% >>build_tmp\out.txt
