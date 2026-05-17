#include <bits/stdc++.h>
#include <chrono>

#define ll long long
#define vi vector<ll>
#define vvi vector<vector<ll>>
#define ii pair<int, int>
#define pb push_back
#define f first
#define s second

using namespace std;
using namespace std::chrono;

ll score;

void swap(ll* a, ll* b) {
  ll temp = *a;
  *a = *b;
  *b = temp;
  return;
}

void transpose(vvi &matrix) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < i; j++) {
      swap(&matrix[i][j], &matrix[j][i]);
    }
  }

  return;
}

void reverse(vvi &matrix) {
  for(int i = 0; i < 4; i++) {
    swap(&matrix[i][3], &matrix[i][0]);
    swap(&matrix[i][2], &matrix[i][1]);
  }
  return;
}

void mergeLeft(vvi &matrix) {
  for(int i = 0; i < 4; i++) {
    // basically the leftmost blocks that can merge, merge.
    // The goal is to find a pair of duplicate elements, and merge them. In case, there are three of them,
    // we merge the leftmost ones.
    int last = -1;
    for(int j = 0; j < 4; j++) {
      if(matrix[i][j] == 0) continue;
      if(last == -1) {
        last = j;
        continue;
      }
      
      if(matrix[i][j] == matrix[i][last]) { //then these two will be merged for sure.
        // printMatrix(matrix);
        // cout<<i<<" "<<last<<endl;
        // cout<<matrix[i][last]<<endl;
        matrix[i][last] *= 2;
        score += matrix[i][last];
        matrix[i][j] = 0;
        last = -1;
      }

      else last = j;
    }

    int ind = 0;
    for(int j = 0; j < 4; j++) {
      if(matrix[i][j] == 0) continue;
      matrix[i][ind] = matrix[i][j];
      if(ind != j) matrix[i][j] = 0;
      ind++;
    }
  }
}

void mergeRight(vvi &matrix) {
    reverse(matrix);
    mergeLeft(matrix);
    reverse(matrix);
}

void mergeUp(vvi &matrix) {
    transpose(matrix);
    mergeLeft(matrix);
    transpose(matrix);
}

void mergeDown(vvi &matrix) {
    reverse(matrix);
    transpose(matrix);
    reverse(matrix);
    mergeLeft(matrix);
    reverse(matrix);
    transpose(matrix);
    reverse(matrix);
}

bool checkEnd(vvi &matrix) {
  ll temp = score;
  bool flag = true;
  vvi tempMatrix = matrix;
  mergeLeft(matrix);
  if(matrix != tempMatrix) flag = false;
  matrix = tempMatrix;
  mergeRight(matrix);
  if(matrix != tempMatrix) flag = false;
  matrix = tempMatrix;
  mergeUp(matrix);
  if(matrix != tempMatrix) flag = false;
  matrix = tempMatrix;
  mergeDown(matrix);
  if(matrix != tempMatrix) flag = false;
  matrix = tempMatrix;

  score = temp;

  return flag;
}

int points(vvi matrix) {
  vvi weights = {
    {2, 4, 8, 16},
    {4, 8, 16, 32},
    {8, 16, 32, 64},
    {16, 32, 64, 128}
  };
  
  ll maxi = 0;
  int countZero = 0;
  ll result = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
       maxi = max(maxi, matrix[i][j]);
       if(matrix[i][j] == 0) countZero++;
       result += (matrix[i][j] * weights[i][j]);
    }
  }

  // bool flag = true;
  // for(int i = 0; i < 3; i++) {
  //   if(matrix[3][i] > matrix[3][i+1]) flag = false;
  //   if(matrix[i][3] > matrix[i+1][3]) flag = false;
  // }

  // result += (flag*maxi*1280);
  result += (countZero*maxi);
  return result;
}

pair<ll, vector<char>> expectimax(vvi& matrix, int depth, bool player) {
  if(depth == 0 || checkEnd(matrix)) return {points(matrix), {'d', 'r', 'u', 'l'}};
  ll result = 0;
  
  vector<pair<ll, char>> moves = {{0, 'd'}, {0, 'r'}, {0, 'u'}, {0, 'l'}};
  
  if(player) {
    ll temp = score;
    vvi tempMatrix = matrix;
    mergeLeft(matrix);
    ll recur = 0;
    if(tempMatrix != matrix) {
      recur = expectimax(matrix, depth-1, false).f;
      moves[0] = {recur, 'l'};
    }

    else {
      moves[0] = {0, 'l'};
    }

    matrix = tempMatrix;
    mergeRight(matrix);
    if(tempMatrix != matrix) {   
      recur = expectimax(matrix, depth-1, false).f;
      moves[1] = {recur, 'r'};
    }

    else {
      moves[1] = {0, 'r'};
    }

    matrix = tempMatrix;
    mergeUp(matrix);
    if(tempMatrix != matrix) {
      recur = expectimax(matrix, depth-1, false).f;
      moves[2] = {recur, 'u'};
    }

    else {
      moves[2] = {0, 'u'};
    }

    matrix = tempMatrix;
    mergeDown(matrix);
    if(tempMatrix != matrix) {
      recur = expectimax(matrix, depth-1, false).f;
      moves[3] = {recur, 'd'}; 
    }

    else {
      moves[3] = {0, 'd'};
    }

    score = temp;
    sort(moves.begin(), moves.end());
    result = moves[3].f;
    matrix = tempMatrix;
  }

  else {
    int countZero = 0;
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        if(matrix[i][j] != 0) continue;
        countZero++;
        matrix[i][j] = 4;
        result += 1*expectimax(matrix, depth - 1, true).f;
        matrix[i][j] = 2;
        result += 9*expectimax(matrix, depth - 1, true).f;
        matrix[i][j] = 0;
      }
    }
    
    if(countZero != 0) 
      result = result / (countZero * 10);
  }

  vector<char> foo(4);
  for(int i = 0; i < 4; i++) {
    foo[i] = moves[3-i].s;
  }


  return {result, foo};
}

int main() {
  auto start = high_resolution_clock::now();
  srand(time(0));
  score = 0;
  vector<vector<ll>> matrix(4, vi(4, 0));

  // fillTile(matrix);
  // fillTile(matrix);

  // printMatrix(matrix);
  //

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      cin>>matrix[i][j];
    }
  }

  if(checkEnd(matrix)) return 0;

  // while(!checkEnd(matrix)) {
    pair<ll, vector<char>> dump = expectimax(matrix, 6, true);
    vector<char> moves = dump.s;

    // cout<<moves[0]<<endl;
    char move = moves[0];

    while(true) {
      if(move == 'l') {
        vvi tempMatrix(4, vi(4));
        ll temp = score;
        tempMatrix = matrix;
        mergeLeft(matrix);
        if(tempMatrix == matrix) {
          score = temp;
          for(int i = 0; i < 4; i++) {
            if(moves[i] == 'l') move = moves[i+1]; //I don't think segfault will occur.
          } 
        }

        else break;
      }

      if(move == 'r') {
        ll temp = score;
        vvi tempMatrix = matrix;
        mergeRight(matrix);
        if(tempMatrix == matrix) {
          score = temp;
          for(int i = 0; i < 4; i++) {
            if(moves[i] == 'r') move = moves[i+1];
          }
        }

        else 
          break;
      }
    
      if(move == 'u') {
        ll temp = score;
        vvi tempMatrix = matrix;
        mergeUp(matrix);
        if(tempMatrix == matrix) {
          score = temp;
          for(int i = 0; i < 4; i++) {
            if(moves[i] == 'u') move = moves[i+1];
          }
        }

        else 
          break;
      }

      if(move == 'd') {
        ll temp = score;
        vvi tempMatrix = matrix;
        mergeDown(matrix);
        if(tempMatrix == matrix) {
          score = temp;
          for(int i = 0; i < 4; i++) {
            if(moves[i] == 'd') move = moves[i+1];
          }
        }

        else 
          break;
      }
  }

  cout<<move;

    // cout<<score<<endl;
    // // fillTile(matrix);
    // cout<<endl;
    // printMatrix(matrix);
  // }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);
  // cout<<"Time taken: "<<duration.count()<<" ms"<<endl;
  return 0;
}
