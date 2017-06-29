#ifndef GAME_H
#define GAME_H

#include <vector>
#include <utility>
#include <string>
using std::wstring;

enum LEVEL{ PRIMARY, MIDDLE, ADVANCED };   //表示游戏级别

class Game{

public:
	Game(LEVEL l = PRIMARY, const int m = 10, const int x = 9, const int y = 9, int r = 81);
	~Game();
	bool Left_button(const int x, const int y);
	void Right_button(const int x, const int y);
	const LEVEL Get_level() { return m_e_level; }
	const int Get_mine_count() { return m_n_mine_count; }
	const int Get_side_x() { return m_n_side_x; }
	const int Get_side_y() { return m_n_side_y; }
	const int Get_rest() { return m_n_rest; }
	int **Get_client_num() { return m_ppn_grid_num; }
	int **Get_client_flag() { return m_ppn_grid_flag; }
	wstring **Get_client_char() { return m_pps_grid_char; }
	void Show_all();

private:
	void Set_mine();
	int Random(const int max);
	void Init_num();
	void Set_mine_round_num(const int i, const int j);

private:
	const LEVEL m_e_level;
	const int m_n_mine_count;
	const int m_n_side_x;
	const int m_n_side_y;
	int m_n_rest;
	int **m_ppn_grid_num;
	int **m_ppn_grid_flag;
	wstring **m_pps_grid_char;
	std::vector<std::pair<int, int> > m_v_mine_pos;
};

#endif