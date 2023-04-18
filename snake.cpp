#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <deque>
#include <vector>
using namespace std;

//snake 와 wall을 x,y 좌표를 갖는 구조체로 선언함
struct Snake{
  int x;
  int y;
  Snake(int a, int b): x(a), y(b){}
};

struct Wall{
  int x;
  int y;
  Wall(int a, int b): x(a), y(b){}
};

struct point{
  int x;
  int y;
  point(int a=0, int b=0):x(a), y(b){}
};

int Level = 1; // 전역변수로 Level 선언하기

//stage별 벽들의 집
vector<vector<Wall>> stages_walls;


void make_wall(){
  vector<Wall> temp;

  //stage 1 wall 만들기
  int wall_x = 20;
  int wall_y = 20;
  for(wall_x; wall_x < 71; wall_x++){
    temp.push_back(Wall(wall_x, wall_y));
  }
  stages_walls.push_back(temp);
  temp.clear();

  //stage 2 wall 만들기
  wall_y = 10;
  wall_x = 20;
  for(wall_x; wall_x < 71; wall_x++){
    temp.push_back(Wall(wall_x, wall_y));
  }
  wall_y = 30;
  wall_x = 20;
  for(wall_x; wall_x < 71; wall_x++){
    temp.push_back(Wall(wall_x, wall_y));
  }
  stages_walls.push_back(temp);
  temp.clear();

  //stage 3 wall 만들기
  temp = stages_walls[1];
  wall_x = 45;
  wall_y = 11;
  for(;wall_y < 30; wall_y++){
    temp.push_back(Wall(wall_x,wall_y));
  }
  stages_walls.push_back(temp);
  temp.clear();

  //stage 4 wall 만들기
  temp = stages_walls[1];
  wall_y = 11;
  wall_x = 20;
  for(;wall_y<30; wall_y++){
    temp.push_back(Wall(wall_x,wall_y));
  }
  wall_y = 11;
  wall_x = 70;
  for(;wall_y<30; wall_y++){
    temp.push_back(Wall(wall_x,wall_y));
  }
  stages_walls.push_back(temp);
}


bool can_be_point(point p){
  bool ret = false;
  vector<Wall> temp = stages_walls[Level - 1];
  for(auto i : temp){
    if(i.x = p.x && i.y == p.y){
      ret = true;
      break;
    }
  }
  return ret;
}

//아이템이 나올 임의이 좌표를 구함 벽좌표는 제외
point make_point(){
  point ret = point(rand() % 88 + 1,  rand() % 38 + 1);
  if(can_be_point(ret)){
    ret = make_point();
  }
  return ret;
}

//벽을 만나는지 아닌지 확인하는 함
bool meet_wall(int x, int y){
  for(auto i : stages_walls[Level - 1]){
    if(i.x == x && i.y == y){
      return true;
    }
  }
  return false;
}

int main(){
  make_wall();
  point p;
  WINDOW* GameWindow;  //게임이 진행되는 화면
  WINDOW* ScoreWindow; //점수가 보여지는 화면
  WINDOW* MissionWindow; //미션이 보여지는 화면

  initscr(); //NCURSES 초기화 함수 선언문?
  noecho(); //사용자의 입력을 안보이게
  curs_set(0); //화면에서 보이는 커서를 설정한다 인자 값이 0일떄 : 커서가 안보이게
  keypad(stdscr, true);
  timeout(200);
  srand((unsigned int) time(NULL));

  start_color();
  //사용할 색들 미리 프리셋으로 선언해놓기
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK); //폰트, 배
  init_pair(2, COLOR_BLACK, COLOR_YELLOW); //mission board
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW); //snake head
  init_pair(4, COLOR_GREEN,COLOR_GREEN); //snake body
  init_pair(5, COLOR_WHITE,COLOR_WHITE); //wall
  init_pair(6, COLOR_BLUE,COLOR_BLUE); //Gate
  init_pair(7, COLOR_RED, COLOR_RED); //poison
  init_pair(8, COLOR_BLACK, COLOR_BLACK); // corner

  deque<Snake> snakes;
  vector<Wall> wall;

  time_t item_in, item_out;
  time_t gameStart, gameEnd;
  time_t super_time; // stage 이동시 무적 시간
  time_t temp_time;

  bool quit = false;
  int score = 0; //성장(Growth) - 독(Poison) 최종점수
  int Growth_cnt = 0, Poison_cnt = 0;
  int gate_cnt = 0; //gate사용 횟수


  int dir = 2; // 방향 정하는 변수 2일 경우 오른쪽으로 이동
  int end_dir = 0; // end_dir 0으로 초기화 시켜주기 나중에 값 넣어줌

  p = make_point();
  int growth_x = p.x; //rand() % 88 + 1;
  int growth_y = p.y; //rand() % 38 + 1;

  p = make_point();
  int poison_x = p.x;
  int poison_y = p.y;

  //gate 좌표 구현
  int gate1_x = rand() % 87 + 1;
  int gate1_y = 0;
  int gate2_x = rand() % 87 + 1;
  int gate2_y = 39;

  int mission_size = 5;
  int mission_growth = 2;
  int mission_poison = 8;

  int key; //입력 방항키
  int snake_sz = 3;

  for(int i=0; i < snake_sz; i++)
    snakes.push_front(Snake(snake_sz+i, snake_sz));

  time(&item_out);
  time(&gameStart);

  while(!quit && Level < 5) { // key 입력 처
    time(&temp_time);
    if(temp_time - super_time <  3){ // stage넘어가기 전 준비시간
      erase();
      mvprintw(1,1,"Ready...");
      continue;
    }

    key = getch();

      switch (key) {
        case KEY_LEFT:
          if(dir != 2) dir = 1; // 현재 진행의 반대방향이 아닐 때만 dir 바꿔줌
          else end_dir = 1; // 반대 키 입력 경우
          break;
        case KEY_RIGHT:
          if(dir != 1) dir = 2;
          else end_dir = 1;
          break;
        case KEY_UP:
          if(dir != 4) dir = 3;
          else end_dir = 1;
          break;
        case KEY_DOWN:
          if(dir != 3) dir = 4;
          else end_dir = 1;
          break;
      }


  if(end_dir == 1) quit = true; //반대키입력을 해서 게임종료

  Snake head = snakes.front();
  int x = head.x, y = head.y;

  if(dir == 1) x--;
  else if(dir == 2) x++;
  else if(dir == 3) y--;
  else if(dir == 4) y++;

  GameWindow = newwin(40, 90, 1, 1);
  wbkgd(GameWindow, COLOR_PAIR(1));
  wattron(GameWindow, COLOR_PAIR(1));
  wborder(GameWindow, COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(8), COLOR_PAIR(8), COLOR_PAIR(8), COLOR_PAIR(8));

  for(auto i : snakes){ // 자기 몸통 만나면 죽는 코드
    if(x == i.x && y == i.y) quit = true;
  }


  if(x == growth_x && y == growth_y){  // growth먹을때 snake_pop_back 하지 않음
    p = make_point();
    growth_x = p.x;
    growth_y = p.y;
    score++; Growth_cnt++;
  }
  else if(meet_wall(x, y)){
    quit = true;
  }
  else if(x == poison_x && y == poison_y){ // poison먹을때 꼬리 2개 pop 해주기
    poison_x = rand() % 88 + 1;
    poison_y = rand() % 38 + 1;
    snakes.pop_back();
    snakes.pop_back();
    Poison_cnt++; score--;
  }
  else if(x == gate1_x && y == gate1_y){
      gate_cnt++;
      if(gate2_x == 0 || gate2_x == 89 || gate2_y == 0 || gate2_y == 39){ // gate2도 가장자리.
      snakes.pop_back();
      x = gate2_x;
      y = gate2_y;

      if (x == 0){
       dir = 2;
       x++;
      }
      else if (x == 89){
        dir = 1;
        x--;
      }
      else if (y == 0){
        dir = 4;
        y++;
      }
        else if (y == 39){
          dir = 3;
          y--;
        }
      }

      else{ // gate2가 가장자리 아닐때.
        if((gate1_x == 15 && gate2_y == 20)){ //wall에 gate 2개 다 있을때
          if(dir == 2){ //오른쪽으로 진입했을때 아래로 내려가기
            snakes.pop_back();
            x = gate2_x;
            y = gate2_y;
            y++;
            dir = 4;
          }
          else if(dir == 1){ //왼쪽으로 진입했을 때 위로 올라가기
            snakes.pop_back();
            x = gate2_x;
            y = gate2_y;
            y--;
            dir = 3;
          }
          else if(dir == 4){ // 위에서 진입했을 때 아래로 내려가기
            snakes.pop_back();
            x = gate2_x;
            y = gate2_y;
            y++;
          }
        }
        else if(gate2_x == 15 && gate1_y == 20){
          if(dir == 4){ // 위에서 진입했을 때 왼쪽으로 가기
            snakes.pop_back();
            x = gate2_x;
            y = gate2_y;
            x--;
            dir = 1;
          }
          else if(dir == 3){ //아래에서 진입했을 때 오른쪽으로 가기
            snakes.pop_back();
            x = gate2_x;
            y = gate2_y;
            x++;
            dir = 2;
          }
        }

        else{
          snakes.pop_back();
          x = gate2_x;
          y = gate2_y;

          if(dir == 1) x--;
          else if(dir == 2) x++;
          else if(dir == 3) y--;
          else if(dir == 4) y++;
        }
      }
  }
  else if(x == gate2_x && y == gate2_y){
    gate_cnt++;
      if(gate1_x == 0 || gate1_x == 89 || gate1_y == 0 || gate1_y == 39){
          snakes.pop_back();
          x = gate1_x;
          y = gate1_y;

          if (x == 0){
          dir = 2;
          x++;
          }
          else if (x == 89){
            dir = 1;
            x--;
          }
          else if (y == 0){
            dir = 4;
            y++;
          }
          else if (y == 39){
            dir = 3;
            y--;
          }
      }
      else{ // gate1이 가장자리 아닐때.
        if(gate2_x == 15 && gate1_y == 20){ //wall에 gate 2개 다 있을때
          if(dir == 2){ //오른쪽으로 진입했을때 아래로 내려가기
            snakes.pop_back();
            x = gate1_x;
            y = gate1_y;
            y++;
            dir = 4;
          }
          else if(dir == 1){ //왼쪽으로 진입했을 때 위로 올라가기
            snakes.pop_back();
            x = gate1_x;
            y = gate1_y;
            y--;
            dir = 3;
          }
          else if(dir == 4){ // 위에서 진입했을 때 아래로 내려가기
            snakes.pop_back();
            x = gate1_x;
            y = gate1_y;
            y++;
          }
        }

        else if(gate1_x == 15 && gate2_y == 20){
          if(dir == 4){ // 위에서 진입했을 때 왼쪽으로 가기
            snakes.pop_back();
            x = gate1_x;
            y = gate1_y;
            x--;
            dir = 1;
          }
          else if(dir == 3){ //아래에서 진입했을 때 오른쪽으로 가기
            snakes.pop_back();
            x = gate1_x;
            y = gate1_y;
            x++;
            dir = 2;
          }
        }

        else{
        snakes.pop_back();
        x = gate1_x;
        y = gate1_y;

        if(dir == 1) x--;
        else if(dir == 2) x++;
        else if(dir == 3) y--;
        else if(dir == 4) y++;
        }
      }
  }
  else
    snakes.pop_back(); // 좌표다 못맞추었을 때 tail자름.

  for(int i=0; i<stages_walls[Level-1].size(); i++){ //임이의 벽 생성하기
    mvwaddch(GameWindow, stages_walls[Level-1][i].y, stages_walls[Level-1][i].x, COLOR_PAIR(5));
  }

  mvwaddch(GameWindow, gate1_y, gate1_x, COLOR_PAIR(6));
  mvwaddch(GameWindow, gate2_y, gate2_x, COLOR_PAIR(6));

  snakes.push_front(Snake(x, y)); //snake 이동 시키기

  if(y > 38 || x > 87 || y < 1 || x < 1) quit = true; // snake가 창 끝에 도달하면 게임종료

  mvwaddch(GameWindow, growth_y, growth_x, COLOR_PAIR(4)); // food 지점 출력
  mvwaddch(GameWindow, poison_y, poison_x, COLOR_PAIR(7)); //poison 지점 출력
  wrefresh(GameWindow);

  time(&item_in);

  if((item_in - item_out) >= 5){ // item, gate들을 시간이 지나면 위치 바꿔주기
    p = make_point();
    growth_x = p.x;
    growth_y = p.y;
    p = make_point();
    poison_x = p.x;
    poison_y = p.y;

    gate1_x = rand() % 87 + 1;
    gate1_y = 0;
    gate2_x = rand() % 87 + 1;
    gate2_y = 39;

    item_out = item_in;
  }

  int head_idx = 0;
  for(auto i : snakes){
    if(head_idx == 0){
        mvwaddch(GameWindow, i.y, i.x, COLOR_PAIR(3));
        head_idx++;
    }
    else mvwaddch(GameWindow, i.y, i.x, COLOR_PAIR(4));
  }
  wrefresh(GameWindow);

  //ScoreWindow 설정
  ScoreWindow = newwin(18, 34, 1, 95);
  wbkgd(ScoreWindow, COLOR_PAIR(2));
  wattron(ScoreWindow, COLOR_PAIR(2));
  wborder(ScoreWindow, '.', '.', '.', '.', '#', '#', '#', '#');
  mvwprintw(ScoreWindow, 1, 1, "<< Score Board >>");
  mvwprintw(ScoreWindow, 3, 1, "SnakeSize : %i", snakes.size());
  mvwprintw(ScoreWindow, 5, 1, "Growth_Count : %i", Growth_cnt);
  mvwprintw(ScoreWindow, 6, 1, "Poison_Count : %i", Poison_cnt);
  mvwprintw(ScoreWindow, 7, 1, "Gate_Count : %i", gate_cnt);
  mvwprintw(ScoreWindow, 9, 1, "Present LEVEL : %i", Level);
  mvwprintw(ScoreWindow, 15, 1, "Total_Score : %i", score);
  wrefresh(ScoreWindow);

  //MissionWindow 설정
  MissionWindow = newwin(19, 34, 22, 95);
  wbkgd(MissionWindow, COLOR_PAIR(2));
  wattron(MissionWindow, COLOR_PAIR(2));
  wborder(MissionWindow, '.', '.', '.', '.', '#', '#', '#', '#');
  mvwprintw(MissionWindow, 1, 1, "<< Mission >>");
  mvwprintw(MissionWindow, 5, 1, "SnakeSize : %i / %i",snakes.size(), mission_size);
  mvwprintw(MissionWindow, 6, 1, "Must get GrowItem : %i / %i",Growth_cnt, mission_growth);
  mvwprintw(MissionWindow, 9, 1, "Don't get PoisonItem : %i / %i",Poison_cnt, mission_poison);
  wrefresh(MissionWindow);

  if(snakes.size() < 3) quit = true; // 길이가 3보다 작으면 게임종료
  if(mission_poison == Poison_cnt) quit = true; // Poison을 한계보다 많이 먹을시 게임 종료


  if(Growth_cnt >= mission_growth && snakes.size() >= mission_size && Level < 5){ // 모든 미션 충족 하고 스테이지가 마지막 스테이가 아닐 경우!
    time(&super_time);

    snakes.clear();
    wall.clear();
    for(int i=0; i<snake_sz; i++) snakes.push_front(Snake(snake_sz+i, snake_sz));
    //미션 난이도 증가 시키기!
    mission_size += 1;
    mission_growth += 1;
    mission_poison -= 2;
    //기존 미션 값들 초기화
    Growth_cnt = 0;
    Poison_cnt = 0;
    gate_cnt = 0;
    //새로운 wall 만들기!

    //stage Level 증가!
    Level++;
    getch();
    dir = 2;
  }

  if(Level == 5){
    //모든 stage를 clear했음
    quit = true;
    //성공완료 출력하기!
  }
  mvprintw(42, 1, "<< If you clear stage of level 4, you will win in the game >>");
  mvprintw(43, 1, "<< You can level up if you clear all mission. >>");
  mvprintw(45, 1, "Present LEVEL : %i", Level);
  refresh();
  }
  time(&gameEnd);
  time_t play_time = gameEnd - gameStart; // 게임 진행 시간
  erase();
  mvprintw(5, 1, "Your Total_Score : %i", score);  //최종 점수 출력
  mvprintw(1, 1, "You gained a total of %i GrowItem.", Growth_cnt);
  mvprintw(2, 1, "You gained a total of %i PoisonItem.", Poison_cnt);
  mvprintw(2, 1, "PlayTime : %i\n", play_time);  //총 게임 실행시간 출력
  refresh();
  getch();
  delwin(GameWindow);
  delwin(ScoreWindow);
  delwin(MissionWindow);
  endwin();
  return 0;
}
