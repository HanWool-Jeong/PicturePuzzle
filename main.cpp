#define _CRT_SECURE_NO_WARNINGS

#include <bangtal.h>
#include <cstring>
#include <cstdio>
#include <ctime>

using namespace bangtal;

// 3x3 ������ �� ���� ��
const int num_of_puzzles = 9;

// ���� ũ�� 200�ȼ�
const int puzzle_size = 200;

// �Ⱥ��� ������ ���� (��ĭ�� �� ���� ����)
const int invisible_puzzle_num = 8;

// ������ �ʱ� ��ǥ
const int x_0 = 374, y_0 = 465;
const int x_1 = x_0 + puzzle_size, y_1 = y_0 - puzzle_size;
const int x_2 = x_1 + puzzle_size, y_2 = y_1 - puzzle_size;

// ���� �Ѱ��� ��ǥ�� ����
struct Position {
	int x, y;
};

// �� ������������ �ʱ� ��ǥ�� ������
Position position[num_of_puzzles] = {
	{ x_0, y_0 },
	{ x_1, y_0 },
	{ x_2, y_0 },
	{ x_0, y_1 },
	{ x_1, y_1 },
	{ x_2, y_1 },
	{ x_0, y_2 },
	{ x_1, y_2 },
	{ x_2, y_2 }
};

// ���� ��ü�� ��ǥ�� ���� ����ü
struct Puzzle {
	Position pos;
	ObjectPtr puzObj;
};

// ���� ���
Puzzle puzzles[num_of_puzzles];

// ���ȭ��
ScenePtr scene;

// �ð� ������ ��� ����
time_t start, end;

// ������ ��ĭ�� �پ��ִ����� �����ϴ� �Լ�
bool isAdjacent(Position puz_pos, Position invisible_pos) {
	// ��ĭ�� �پ� �ִٰ� �˷��ִ� �÷���
	bool adj_flag = false;

	// Ŭ���� ���� �����ʿ� ��ĭ�� ������
	if (puz_pos.x + puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
		adj_flag = true;
	}
	// Ŭ���� ���� ���ʿ� ��ĭ�� ������
	else if (puz_pos.x - puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
		adj_flag = true;
	}
	// Ŭ���� ���� ���� ��ĭ�� ������
	else if (puz_pos.y + puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
		adj_flag = true;
	}
	// Ŭ���� ���� �Ʒ��� ��ĭ�� ������
	else if (puz_pos.y - puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
		adj_flag = true;
	}
	
	return adj_flag;
}

// ������ ��ġ�� ȭ��󿡼� �ٲٴ� �Լ�
void puzzle_swap(Puzzle puzzle, Puzzle invisible)
{
	puzzle.puzObj->locate(scene, puzzle.pos.x, puzzle.pos.y);
	invisible.puzObj->locate(scene, invisible.pos.x, invisible.pos.y);
}

// ������ ��ǥ�� �ٲٴ� �Լ�
void position_swap(Puzzle *puzzle, Puzzle *invisible) {
	int tx = puzzle->pos.x;
	int ty = puzzle->pos.y;

	puzzle->pos.x = invisible->pos.x;
	puzzle->pos.y = invisible->pos.y;

	invisible->pos.x = tx;
	invisible->pos.y = ty;
}

// ��� ������� ���ڸ��� �ִ��� Ȯ��
bool check_alright()
{
	for (int i = 0; i < num_of_puzzles; i++) {
		if (!(puzzles[i].pos.x == position[i].x && puzzles[i].pos.y == position[i].y)) {
			return false;
		}
	}
	return true;
}

// �� �ð��� �޼����� ����ϴ� �Լ�
void show_Time_Used(time_t time_used)
{
	int prev_time = 100000;

	// ������Ʈ ���� ���� ��� �ؽ�Ʈ������ ����
	FILE* record;

	if ((record = fopen("record.txt", "r")) != NULL) {
		// �ؽ�Ʈ���� ���� ����� �����´�.
		fscanf_s(record, "%d", &prev_time);
		fclose(record);
	}
	else {
		showMessage("�ϼ��Ͽ����ϴ�!\n�ùٸ��� �����Ϸ��� record.txt�� �����ϼ���");
		return;
	}

	char str[100];		// ����� ��ü ���ڿ�
	char temp[100];		// ���ڿ� �ӽ� �����

	if (time_used < prev_time) {
		strcpy(temp, "�ְ��� ���!\n");
		if ((record = fopen("record.txt", "r")) != NULL) {
			// ����
			fprintf(record, "%lld", time_used);
			fclose(record);
		}
	}
	else {
		strcpy(temp, "�ϼ��Ͽ����ϴ�!\n");
	}
	sprintf(str, "%s�ɸ��ð�: %lld��", temp, time_used);
	showMessage(str);
}

// ������������ ���콺 �ݹ��� �����ϴ� �Լ�
bool puzzle_mouseCallBack(ObjectPtr object, int x, int y, MouseAction action)
{
	for (int i = 0; i < num_of_puzzles; i++) {
		if (puzzles[i].puzObj == puzzles[invisible_puzzle_num].puzObj)
			continue;

		if (object == puzzles[i].puzObj)
			if (isAdjacent(puzzles[i].pos, puzzles[invisible_puzzle_num].pos)) {
				position_swap(&puzzles[i], &puzzles[invisible_puzzle_num]);
				puzzle_swap(puzzles[i], puzzles[invisible_puzzle_num]);
			}
	}
	if (check_alright()) {
		end = time(NULL);
		show_Time_Used(end - start);
		
		for (int i = 0; i < num_of_puzzles; i++)
			puzzles[i].puzObj->setOnMouseCallback(nullptr);
	}
	
	return true;
}

int main(void)
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	
	auto timer = Timer::create(0.1f);
	int how_many_shuffle = 30;

	scene = Scene::create("�����������", "images/kakao.jpg");
	auto startButton = Object::create("images/start.png", scene, 550, 100);
	startButton->setScale(1.5f);

	// ������ ��ǥ�� �������� ��ü�� ����
	for (int i = 0; i < num_of_puzzles; i++) {
		// ��θ� ����
		std::string path = "images/output/" + std::to_string(i) + ".jpg";

		puzzles[i].puzObj = Object::create(path, scene, position[i].x, position[i].y, false);
		puzzles[i].pos.x = position[i].x;
		puzzles[i].pos.y = position[i].y;
		puzzles[i].puzObj->setOnMouseCallback(puzzle_mouseCallBack);
	}
	
	startButton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		startButton->hide();

		// ��ŸƮ ��ư Ŭ���� �� �ո� ������� �����ǰ�
		scene->setImage("images/kakao_cut.jpg");

		// �� ���� ��ü�� ���̰� �ȴ�.
		for (int i = 0; i < num_of_puzzles; i++) {
			if (i == invisible_puzzle_num)
				continue;
			puzzles[i].puzObj->show();
		}

		// ���� ����
		timer->setOnTimerCallback([&](TimerPtr t)->bool {
			if (how_many_shuffle < 0) {
				start = time(NULL);	// �� ������ �ð� ���� ����
				return true;
			}

			int random;
			do {
				random = rand() % num_of_puzzles;
			} while (!isAdjacent(puzzles[random].pos, puzzles[invisible_puzzle_num].pos));
			position_swap(&puzzles[random], &puzzles[invisible_puzzle_num]);
			puzzle_swap(puzzles[random], puzzles[invisible_puzzle_num]);
			
			how_many_shuffle--;
			timer->set(0.1f);
			timer->start();
			return true;
		});
		timer->start();

		return true;
	});

	startGame(scene);

	return 0;
}