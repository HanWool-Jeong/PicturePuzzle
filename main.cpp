#define _CRT_SECURE_NO_WARNINGS

#include <bangtal.h>
#include <cstring>
#include <cstdio>
#include <ctime>

using namespace bangtal;

// 3x3 퍼즐의 총 조각 수
const int num_of_puzzles = 9;

// 퍼즐 크기 200픽셀
const int puzzle_size = 200;

// 안보일 퍼즐의 숫자 (빈칸이 될 퍼즐 조각)
const int invisible_puzzle_num = 8;

// 퍼즐의 초기 좌표
const int x_0 = 374, y_0 = 465;
const int x_1 = x_0 + puzzle_size, y_1 = y_0 - puzzle_size;
const int x_2 = x_1 + puzzle_size, y_2 = y_1 - puzzle_size;

// 퍼즐 한개의 좌표를 저장
struct Position {
	int x, y;
};

// 각 퍼즐조각마다 초기 좌표를 저장함
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

// 퍼즐 객체와 좌표를 묶은 구조체
struct Puzzle {
	Position pos;
	ObjectPtr puzObj;
};

// 퍼즐 목록
Puzzle puzzles[num_of_puzzles];

// 배경화면
ScenePtr scene;

// 시간 측정이 담길 변수
time_t start, end;

// 퍼즐이 빈칸과 붙어있는지를 리턴하는 함수
bool isAdjacent(Position puz_pos, Position invisible_pos) {
	// 빈칸과 붙어 있다고 알려주는 플래그
	bool adj_flag = false;

	// 클릭된 퍼즐 오른쪽에 빈칸이 있으면
	if (puz_pos.x + puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
		adj_flag = true;
	}
	// 클릭된 퍼즐 왼쪽에 빈칸이 있으면
	else if (puz_pos.x - puzzle_size == invisible_pos.x && puz_pos.y == invisible_pos.y) {
		adj_flag = true;
	}
	// 클릭된 퍼즐 위에 빈칸이 있으면
	else if (puz_pos.y + puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
		adj_flag = true;
	}
	// 클릭된 퍼즐 아래에 빈칸이 있으면
	else if (puz_pos.y - puzzle_size == invisible_pos.y && puz_pos.x == invisible_pos.x) {
		adj_flag = true;
	}
	
	return adj_flag;
}

// 퍼즐의 위치를 화면상에서 바꾸는 함수
void puzzle_swap(Puzzle puzzle, Puzzle invisible)
{
	puzzle.puzObj->locate(scene, puzzle.pos.x, puzzle.pos.y);
	invisible.puzObj->locate(scene, invisible.pos.x, invisible.pos.y);
}

// 퍼즐의 좌표를 바꾸는 함수
void position_swap(Puzzle *puzzle, Puzzle *invisible) {
	int tx = puzzle->pos.x;
	int ty = puzzle->pos.y;

	puzzle->pos.x = invisible->pos.x;
	puzzle->pos.y = invisible->pos.y;

	invisible->pos.x = tx;
	invisible->pos.y = ty;
}

// 모든 퍼즐들이 제자리에 있는지 확인
bool check_alright()
{
	for (int i = 0; i < num_of_puzzles; i++) {
		if (!(puzzles[i].pos.x == position[i].x && puzzles[i].pos.y == position[i].y)) {
			return false;
		}
	}
	return true;
}

// 쓴 시간을 메세지로 출력하는 함수
void show_Time_Used(time_t time_used)
{
	int prev_time = 100000;

	// 프로젝트 폴더 내에 기록 텍스트문서를 연다
	FILE* record;

	if ((record = fopen("record.txt", "r")) != NULL) {
		// 텍스트에서 이전 기록을 가져온다.
		fscanf_s(record, "%d", &prev_time);
		fclose(record);
	}
	else {
		showMessage("완성하였습니다!\n올바르게 동작하려면 record.txt를 포함하세요");
		return;
	}

	char str[100];		// 출력할 전체 문자열
	char temp[100];		// 문자열 임시 저장소

	if (time_used < prev_time) {
		strcpy(temp, "최고기록 경신!\n");
		if ((record = fopen("record.txt", "r")) != NULL) {
			// 쓰기
			fprintf(record, "%lld", time_used);
			fclose(record);
		}
	}
	else {
		strcpy(temp, "완성하였습니다!\n");
	}
	sprintf(str, "%s걸린시간: %lld초", temp, time_used);
	showMessage(str);
}

// 퍼즐조각마다 마우스 콜백을 적용하는 함수
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

	scene = Scene::create("사진퍼즐게임", "images/kakao.jpg");
	auto startButton = Object::create("images/start.png", scene, 550, 100);
	startButton->setScale(1.5f);

	// 저장한 좌표로 퍼즐조각 객체를 만듦
	for (int i = 0; i < num_of_puzzles; i++) {
		// 경로를 만듦
		std::string path = "images/output/" + std::to_string(i) + ".jpg";

		puzzles[i].puzObj = Object::create(path, scene, position[i].x, position[i].y, false);
		puzzles[i].pos.x = position[i].x;
		puzzles[i].pos.y = position[i].y;
		puzzles[i].puzObj->setOnMouseCallback(puzzle_mouseCallBack);
	}
	
	startButton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		startButton->hide();

		// 스타트 버튼 클릭시 뻥 뚫린 배경으로 설정되고
		scene->setImage("images/kakao_cut.jpg");

		// 각 퍼즐 객체가 보이게 된다.
		for (int i = 0; i < num_of_puzzles; i++) {
			if (i == invisible_puzzle_num)
				continue;
			puzzles[i].puzObj->show();
		}

		// 퍼즐 섞기
		timer->setOnTimerCallback([&](TimerPtr t)->bool {
			if (how_many_shuffle < 0) {
				start = time(NULL);	// 다 섞으면 시간 측정 시작
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