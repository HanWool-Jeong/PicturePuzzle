#include <bangtal.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

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
typedef struct Position Position;

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
typedef struct Puzzle Puzzle;

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

void show_Time_Used(time_t time_used)
{
	int prev_time = 100000;

	// 프로젝트 폴더 내에 기록 텍스트문서를 연다
	FILE* record;

	if (fopen_s(&record, "record.txt", "r") == 0) {
		// 텍스트에서 이전 기록을 가져온다.
		fscanf_s(record, "%d", &prev_time);
		fclose(record);
	}
	else {
		showMessage("완성하였습니다!\n올바르게 동작하려면 record.txt를 포함하세요");
		return;
	}

	char s1[100];

	// 걸린 시간 기록을 문자열로 변환
	char t[10];
	_ltoa_s(time_used, t, 10);

	// 이전 기록보다 빨리했으면 새 기록을 텍스트에 쓴다.
	if (time_used < prev_time) {
		strcpy_s(s1, "최고기록 경신!");

		if (fopen_s(&record, "record.txt", "w") == 0) {
			// 쓰기
			fprintf(record, "%s", t);
			fclose(record);
		}
	}
	else {
		strcpy_s(s1, "완성하였습니다!");
	}

	strcat_s(s1, "\n걸린시간: ");
	strcat_s(s1, t);
	strcat_s(s1, "초");

	showMessage(s1);
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

// 퍼즐들을 섞는 함수
void shuffle()
{
	srand(time(NULL));

	for (int i = 0; i < num_of_puzzles; i++) {
		int random = rand() % 9;
		position_swap(&puzzles[i], &puzzles[random]);
		puzzle_swap(puzzles[i], puzzles[random]);
	}
}

int main(void)
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

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
		//shuffle();

		// 시간 측정
		start = time(NULL);
		return true;
	});

	startGame(scene);
	return 0;
}