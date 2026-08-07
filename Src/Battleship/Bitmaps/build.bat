set bat_generator_path=./../../../../STM32F4_Libraries/Utils/main.py

python3 %bat_generator_path% BM_BattleshipLogo.png BM_BattleshipLogo --struct
python3 %bat_generator_path% BM_Ship.png BM_Ship --struct
python3 %bat_generator_path% BM_DeadCell.png BM_DeadCell --struct --invert
python3 %bat_generator_path% BM_EmptyCell.png BM_EmptyCell --struct --invert
python3 %bat_generator_path% BM_Bomb_0.png BM_Bomb_0 --struct
python3 %bat_generator_path% BM_Bomb_1.png BM_Bomb_1 --struct
python3 %bat_generator_path% BM_Bomb_2.png BM_Bomb_2 --struct
python3 %bat_generator_path% BM_Bomb_3.png BM_Bomb_3 --struct
python3 %bat_generator_path% BM_Bomb_4.png BM_Bomb_4 --struct
python3 %bat_generator_path% BM_Bomb_5.png BM_Bomb_5 --struct
python3 %bat_generator_path% BM_PopalText.png BM_PopalText --struct
python3 %bat_generator_path% BM_UbilText.png BM_UbilText --struct