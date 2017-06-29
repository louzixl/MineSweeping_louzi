#include "Game.h"
#include <random>
#include <ctime>
#include <windows.h>

Game::Game(LEVEL l, const int m, const int x, const int y, int r)
	:m_e_level(l), m_n_mine_count(m), m_n_side_x(x), m_n_side_y(y), m_n_rest(r)
{
	int i, j;

	m_ppn_grid_num = new int*[y];            //分配内存
	m_ppn_grid_flag = new int*[y];
	m_pps_grid_char = new wstring*[y];
	for (i = 0; i < y; i++)
	{
		m_ppn_grid_num[i] = new int[x];
		m_ppn_grid_flag[i] = new int[x];
		m_pps_grid_char[i] = new wstring[x];
	}

	for (i = 0; i < y; i++)                  //将数组置为0
	{
		for (j = 0; j < x; j++)
		{
			m_ppn_grid_num[i][j] = 0;
			m_ppn_grid_flag[i][j] = 0;
		}
	}

	Set_mine();    //随机放置雷
	Init_num();    //初始化数字
}

Game::~Game()
{
	int i;

	for (i = 0; i < m_n_side_y; i++)         //清理内存
	{
		delete[] m_ppn_grid_num[i];
		delete[] m_ppn_grid_flag[i];
		delete[] m_pps_grid_char[i];
	}
	delete[] m_ppn_grid_num;
	delete[] m_ppn_grid_flag;
	delete[] m_pps_grid_char;
}

bool Game::Left_button(const int x, const int y)
{
	if (x < 0 || x >= m_n_side_x || y < 0 || y >= m_n_side_y)
		return true;
	if (m_ppn_grid_num[y][x] == -1)
	{
		return false;
	}
	else if (m_ppn_grid_flag[y][x] == 0 || m_ppn_grid_flag[y][x] == 2)
	{
		m_ppn_grid_flag[y][x] = 1;
		m_n_rest--;
	}
	return true;
}

void Game::Right_button(const int x, const int y)
{
	if (x < 0 || x >= m_n_side_x || y < 0 || y >= m_n_side_y)
		return;
	if (m_ppn_grid_flag[y][x] == 0)
	{
		m_ppn_grid_flag[y][x] = 2;
	}
	else if (m_ppn_grid_flag[y][x] == 1)
		return;
	else if (m_ppn_grid_flag[y][x] == 2)
	{
		m_ppn_grid_flag[y][x] = 0;
	}
}

void Game::Set_mine()
{
	int i, x, y;

	for (i = 0; i < m_n_mine_count;)
	{
		y = Random(m_n_side_x*m_n_side_y - 1) / m_n_side_x;
		x = Random(m_n_side_x*m_n_side_y - 1) % m_n_side_x;
		if (m_ppn_grid_num[y][x] == -1)
			continue;
		else
		{
			m_ppn_grid_num[y][x] = -1;
			m_v_mine_pos.push_back(std::make_pair(x, y));
			i++;
		}
	}
}

int Game::Random(const int max)
{
	std::default_random_engine engine(time(0));
	return std::uniform_int_distribution<int>(0, max)(engine);
}

void Game::Init_num()
{
	int x, y;
	std::vector<std::pair<int, int>>::const_iterator pos = m_v_mine_pos.cbegin();
	std::vector<std::pair<int, int>>::const_iterator end = m_v_mine_pos.cend();

	for (pos; pos != end; ++pos)
	{
		x = pos->first;
		y = pos->second;
		Set_mine_round_num(y, x);
	}
}

void Game::Set_mine_round_num(const int i, const int j)
{
	if (i > 0 && m_ppn_grid_num[i - 1][j] != -1)
	{
		m_ppn_grid_num[i - 1][j]++;
		if (j > 0 && m_ppn_grid_num[i - 1][j - 1] != -1)
			m_ppn_grid_num[i - 1][j - 1]++;
		if (j < m_n_side_x - 1 && m_ppn_grid_num[i - 1][j + 1] != -1)
			m_ppn_grid_num[i - 1][j + 1]++;
	}
	if (i < m_n_side_y - 1 && m_ppn_grid_num[i + 1][j] != -1)
	{
		m_ppn_grid_num[i + 1][j]++;
		if (j > 0 && m_ppn_grid_num[i + 1][j - 1] != -1)
			m_ppn_grid_num[i + 1][j - 1]++;
		if (j < m_n_side_x - 1 && m_ppn_grid_num[i + 1][j + 1] != -1)
			m_ppn_grid_num[i + 1][j + 1]++;
	}
	if (j > 0 && m_ppn_grid_num[i][j - 1] != -1)
		m_ppn_grid_num[i][j - 1]++;
	if (j < m_n_side_x - 1 && m_ppn_grid_num[i][j + 1] != -1)
		m_ppn_grid_num[i][j + 1]++;
}

void Game::Show_all()
{
	int i, j;

	for (i = 0; i < m_n_side_y; i++)
	{
		for (j = 0; j < m_n_side_x; j++)
		{
			if (m_ppn_grid_num[i][j] >= 0)
				m_ppn_grid_flag[i][j] = 1;
			else
				m_ppn_grid_flag[i][j] = 3;
		}
	}
}