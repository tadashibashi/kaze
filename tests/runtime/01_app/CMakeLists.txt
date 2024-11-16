
add_kaze_executable(ktest_01_app KAZE_TK
    COMPANY_NAME kaze
    APP_NAME "Kaze App Test"
    ICON_FILE assets/dungeon_tiles.png
    ICON_FILE_ICO icon.ico
    SOURCES
        src/01_app.cpp)
kaze_target_assets(ktest_01_app
    ASSET_DIR assets
    ASSETS
        dungeon_tiles.png
        computer_score.wav
        player_score.wav
)
