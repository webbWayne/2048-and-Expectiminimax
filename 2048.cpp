#include <bits/stdc++.h>

#define ll long long
#define vi vector<ll>
#define vvi vector<vector<ll>>
#define ii pair<int, int>
#define pb push_back
#define f first
#define s second

using namespace std;

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

void printMatrix(vvi matrix) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      cout<<matrix[i][j]<<" ";
    }
    cout<<endl;
  }

  return;
}

vvi mergeLeft(vvi matrix) {
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

  return matrix;
}

vvi mergeRight(vvi matrix) {
    reverse(matrix);
    matrix = mergeLeft(matrix);
    reverse(matrix);
    return matrix;
}

vvi mergeUp(vvi matrix) {
    transpose(matrix);
    matrix = mergeLeft(matrix);
    transpose(matrix);
    return matrix;
}

vvi mergeDown(vvi matrix) {
    reverse(matrix);
    transpose(matrix);
    reverse(matrix);
    matrix = mergeLeft(matrix);
    reverse(matrix);
    transpose(matrix);
    reverse(matrix);
    return matrix;
}

bool checkEnd(vvi matrix) {
  ll temp = score;
  bool flag = false;
  if(mergeLeft(matrix) == matrix && mergeRight(matrix) == matrix
&& mergeUp(matrix) == matrix && mergeDown(matrix) == matrix) flag = true;

  score = temp;

  return flag;
}

vvi fillTile(vvi matrix) {
  vector<ii> freeTiles;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++)
      if(matrix[i][j] == 0) freeTiles.pb({i, j});
  }

  int randInd = rand() % freeTiles.size();

  bool flag = ((rand() % 100) < 90);
  if(flag) {
    matrix[freeTiles[randInd].f][freeTiles[randInd].s] = 2;
  }

  else {
    matrix[freeTiles[randInd].f][freeTiles[randInd].s] = 4;
  }

  return matrix;
}

int points(vvi matrix) {
  ll maxi = 0;
  int countZero = 0;
  ll result = 0;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
       maxi = max(maxi, matrix[i][j]);
       if(matrix[i][j] == 0) countZero++;
       result += (matrix[i][j] * (i+1) * (j+1));
    }
  }

  result += (countZero*maxi);
  return result;
}

pair<ll, vector<char>> expectimax(vvi matrix, int depth, bool player) {
  if(depth == 0 || checkEnd(matrix)) return {points(matrix), {'d', 'r', 'u', 'l'}};
  ll result = 0;
  
  vector<pair<ll, char>> moves = {{0, 'd'}, {0, 'r'}, {0, 'u'}, {0, 'l'}};
  
  if(player) {
    ll temp = score;
    vvi tempMatrix = mergeLeft(matrix);
    ll recur = 0;
    if(tempMatrix != matrix) {
      recur = expectimax(tempMatrix, depth-1, false).f;
      moves[0] = {recur, 'l'};
    }

    else {
      moves[0] = {-1000, 'l'};
    }

    tempMatrix = mergeRight(matrix);
    if(tempMatrix != matrix) {
      recur = expectimax(tempMatrix, depth-1, false).f;
      moves[1] = {recur, 'r'};
    }

    else {
      moves[1] = {0, 'r'};
    }

    tempMatrix = mergeUp(matrix);
    if(tempMatrix != matrix) {
      recur = expectimax(tempMatrix, depth-1, false).f;
      moves[2] = {recur, 'u'};
    }

    else {
      moves[2] = {0, 'u'};
    }
  
    tempMatrix = mergeDown(matrix);
    if(tempMatrix != matrix) {
      recur = expectimax(tempMatrix, depth-1, false).f;
      moves[3] = {recur, 'd'}; 
    }

    else {
      moves[3] = {0, 'd'};
    }

    score = temp;
    sort(moves.begin(), moves.end());
    result = moves[3].f;
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
  srand(time(0));
  score = 0;
  vector<vector<ll>> matrix(4, vi(4, 0));

  matrix = fillTile(matrix);
  matrix = fillTile(matrix);

  printMatrix(matrix);

  while(!checkEnd(matrix)) {
    pair<ll, vector<char>> dump = expectimax(matrix, 7, true);
    vector<char> moves = dump.s;

    cout<<moves[0];
    cout<<endl;
    char move = moves[0];

  while(true) {
    if(move == 'l') {
      ll temp = score;
      vvi tempMatrix = mergeLeft(matrix);
      if(tempMatrix == matrix) {
        score = temp;
        for(int i = 0; i < 4; i++) {
          if(moves[i] == 'l') move = moves[i+1]; //I don't think segfault will occur.
        } 
      }
      else {
        matrix = tempMatrix;
        break;
      }
    }

    if(move == 'r') {
      ll temp = score;
      vvi tempMatrix = mergeRight(matrix);
      if(tempMatrix == matrix) {
        score = temp;
        for(int i = 0; i < 4; i++) {
          if(moves[i] == 'r') move = moves[i+1];
        }
      }

      else {
        matrix = tempMatrix;
        break;
      }
    }
    
    if(move == 'u') {
      ll temp = score;
      vvi tempMatrix = mergeUp(matrix);
      if(tempMatrix == matrix) {
        score = temp;
        for(int i = 0; i < 4; i++) {
          if(moves[i] == 'u') move = moves[i+1];
        }
      }

      else {
        matrix = tempMatrix;
        break;
      }
    }

    if(move == 'd') {
      ll temp = score;
      vvi tempMatrix = mergeDown(matrix);
      if(tempMatrix == matrix) {
        score = temp;
        for(int i = 0; i < 4; i++) {
          if(moves[i] == 'd') move = moves[i+1];
        }
      }

      else {
        matrix = tempMatrix;
        break;
      }
    }
  }

    cout<<score<<endl;
    matrix = fillTile(matrix);
    cout<<endl;
    printMatrix(matrix);
  }
  return 0;
}
