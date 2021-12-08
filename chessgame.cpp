//chessGame.cpp
//Created by: Matt Hudson

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <array>
#include <stack>
#include <iterator>
#include <tuple>

using namespace std;

struct Location{
    int row;
    int col;
};

// Maps to make things easier for lookup of board spaces
map<int,string> colToLetter = {{0,"A"},{1,"B"},{2,"C"},{3,"D"},{4,"E"},{5,"F"},{6,"G"},{7,"H"}};
map<int,string> rowToRowInt = {{0,"8"},{1,"7"},{2,"6"},{3,"5"},{4,"4"},{5,"3"},{6,"2"},{7,"1"}};
map<string,int> letterToCol = {{"A",0},{"B",1},{"C",2},{"D",3},{"E",4},{"F",5},{"G",6},{"H",7}};
map<string,int> rowIntToRow = {{"8",0},{"7",1},{"6",2},{"5",3},{"4",4},{"3",5},{"2",6},{"1",7}};

// Setup the class for a generic chessPiece
class chessPiece{
    public:
        string name;
        string shortName;
        string pieceNum;
        Location current;
        bool isWhite;
        bool hasMoved = false;
        chessPiece(){
            name = "Unassigned";
        }
        ~chessPiece(){}
        chessPiece(bool _isWhite, string _pieceNum){
            isWhite = _isWhite;
            pieceNum = _pieceNum;
        };

        string getPiece(){
            return name;
        }
        void setPieceNum(int _num){
            pieceNum=to_string(_num);
        }
        string getPieceNum(){
            return pieceNum;
        }
        string getShortName(){
            return shortName;
        }
        bool getIfWhite(){
            return isWhite;
        }
        bool getIfMoved(){
            return hasMoved;
        }
        int getCurrentRow(){
            return current.row;
        }
        int getCurrentCol(){
            return current.col;
        }
        void setCurrentRow(int _row){
            current.row = _row;
        }
        void setCurrentCol(int _col){
            current.col = _col;
        }
        void changeToMoved(){
            if(!hasMoved) hasMoved = true;
        }
};

// Setup the class for a spot on the chessboard
class Spot{
    public:
        bool isOccupied;
        chessPiece *occupyingPiece;
        Spot(){
            isOccupied = false;
            occupyingPiece = new chessPiece;
        };
        void setPiece(chessPiece _chessPiece){
            isOccupied = true;
            occupyingPiece = &_chessPiece;
        }
        void setPointerPiece(chessPiece* _pointer){
            isOccupied = true;
            occupyingPiece = _pointer;
        }
        void removePiece(){
            isOccupied = false;
            occupyingPiece = nullptr;
        }
};

// Setup for a king piece
class King : public chessPiece{
    public:
        int pieceId = 1;
        King(bool _isWhite, string _pieceNum):chessPiece(_isWhite, _pieceNum){
            isWhite = _isWhite;
            name="King";
            shortName = "K";
            pieceNum = _pieceNum;
        }

};
//Setup for a Queen piece
class Queen : public chessPiece{
    public:
        int pieceId = 2;
        Queen(bool _isWhite, string _pieceNum):chessPiece(_isWhite, _pieceNum){
            isWhite = _isWhite;
            name = "Queen";
            shortName = "Q";
            pieceNum = _pieceNum;
        }
      
};
//Setup for a Knight piece
class Knight : public chessPiece{
    public:
        int pieceId = 3;
        Knight(bool _isWhite, string _pieceNum):chessPiece(_isWhite, _pieceNum){
            isWhite = _isWhite;
            name="Knight";
            shortName = "H";
            pieceNum = _pieceNum;
        }
      
};
//Setup for a Bishop piece
class Bishop: public chessPiece{
    public:
        int pieceId = 4;
        Bishop(bool _isWhite, string _pieceNum):chessPiece(_isWhite,_pieceNum){
            isWhite = _isWhite;
            name ="Bishop";
            shortName = "B";
            pieceNum=_pieceNum;
        }
      
};
//Setup for a Rook piece
class Rook : public chessPiece{
    public:
        int pieceId =5;
        Rook(bool _isWhite, string _pieceNum):chessPiece(_isWhite, _pieceNum){
            isWhite = _isWhite;
            name = "Rook";
            shortName = "R";
            pieceNum = _pieceNum;
        }
        void setPieceNum(int i){
            pieceNum = to_string(i);
        }

};
//Set up for a pawn piece
class Pawn : public chessPiece{
    public:
        int pieceId =6;
        Pawn(bool _isWhite, string _pieceNum):chessPiece(_isWhite, _pieceNum){
            isWhite = _isWhite;
            name = "Pawn";
            shortName = "P";
            pieceNum = _pieceNum;
        }

};

void movePiece(Spot board[8][8], chessPiece** _piece, int newRow, int newCol, map<string,chessPiece*>* foePieces){
    /*This function takes a specified piece and moves it on the board. It handles both updating the board spot's piece
    as well as the pieces current location, as well as removing pieces from the opponenets pool of pieces */

    board[(*_piece)->getCurrentRow()][(*_piece)->getCurrentCol()].removePiece();
    bool castle = false;
    if((*_piece)->getPiece() == "King" && abs((*_piece)->getCurrentCol()-newCol) == 2) castle = true;
    //If there is a standard take, remove the opponent piece
    if(board[newRow][newCol].isOccupied){
        string color = "W";
        if((*_piece)->getIfWhite()) color = "B";
        string oldPiece = color+ board[newRow][newCol].occupyingPiece->getShortName() + board[newRow][newCol].occupyingPiece->getPieceNum();
        map<string,chessPiece*>::iterator it = (*foePieces).find(oldPiece);
        (*foePieces).erase(it);
    }
    //Update the board and the piece to reflect the new position
    board[newRow][newCol].isOccupied = true;
    board[newRow][newCol].occupyingPiece = *_piece;
    (*_piece)->setCurrentRow(newRow);
    (*_piece)->setCurrentCol(newCol);
    (*_piece)->changeToMoved();
    //Check for an en passant take by a pawn and execute if that is what happened
    int side = -1;
    if((*_piece)->getIfWhite()) side =1;
    if(board[newRow+side][newCol].isOccupied){
        if((*_piece)->getPiece() == "Pawn" && board[newRow+side][newCol].occupyingPiece->getPiece() == "Pawn"){
            string color = "W";
            if((*_piece)->getIfWhite()) color = "B";
            string oldPiece = color+ board[newRow+side][newCol].occupyingPiece->getShortName() + board[newRow+side][newCol].occupyingPiece->getPieceNum();
            map<string,chessPiece*>::iterator it = (*foePieces).find(oldPiece);
            (*foePieces).erase(it);
            board[newRow+side][newCol].removePiece();
        }
    }
    //Move the rook if the player chose to castle
    if(castle){
        chessPiece** rook;
        //King Side Castle
        if(newCol == 6){
            rook = &board[newRow][7].occupyingPiece;
            board[newRow][5].isOccupied = true;
            board[newRow][5].occupyingPiece = *rook;
            (*rook)->setCurrentCol(5);
            (*rook)->changeToMoved();
            board[newRow][7].removePiece();
        }
        //Queen Side Castle
        else{
            rook = &board[newRow][0].occupyingPiece;
            board[newRow][3].isOccupied = true;
            board[newRow][3].occupyingPiece = *rook;
            (*rook)->setCurrentCol(3);
            (*rook)->changeToMoved();
            board[newRow][0].removePiece();
        }

    }

}

map<string,vector<int>> getPossibleSpots(Spot board[8][8], chessPiece thisPiece, vector<string> history,bool inCheck,map<string,vector<int>> inCheckSpots){
    /* This function returns a map of a string of the board location with a vector of the board spot index that it correlates to.
    It handles the possible locations of all chesspieces*/
    map<string,vector<int>> moves;
    vector<int> temp;
    bool isWhite = thisPiece.isWhite;
    bool hasMoved = thisPiece.hasMoved;
    int row = thisPiece.current.row;
    int col = thisPiece.current.col;
    //get the possible locations of Rook movement
    if(thisPiece.name == "Rook"){
        //up
        for(int i =1;i<8;i++){
            if(row-i < 0) break;
            if(board[row-i][col].isOccupied){
                if(isWhite == board[row-i][col].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col};
            moves.insert({colToLetter.at(col)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col].isOccupied) break;
        }
        //down
        for(int i=1;i<8;i++){
            if(row+i > 7) break;
            if(board[row+i][col].isOccupied){
                if(isWhite == board[row+i][col].occupyingPiece->getIfWhite()) break;
            }
            temp = {row+i,col};
            moves.insert({colToLetter.at(col)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col].isOccupied) break;
        }
        //left
        for(int i=1;i<8;i++){
            if(col-i < 0) break;
            if(board[row][col-i].isOccupied){
                if(isWhite == board[row][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row),temp});
            if(board[row][col-i].isOccupied) break;
        }
        //right
        for(int i=1;i<8;i++){
            if(col+i > 7) break;
            if(board[row][col+i].isOccupied){
                if(isWhite == board[row][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row,col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row),temp});
            if(board[row][col+i].isOccupied) break;
        }
    }
    //Get possible locations of Queen movement
    else if(thisPiece.name == "Queen"){
        //left and up
        for(int i=1;i<8;i++){
            if((row-i) < 0 || (col-i) < 0) break;
            if(board[row-i][col-i].isOccupied){
                if(isWhite == board[row-i][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col-i].isOccupied) break;
        }
        //left and down
        for(int i=1;i<8;i++){
            if((row+i) > 7 || (col-i) < 0) break;
            if(board[row+i][col-i].isOccupied){
                if(isWhite == board[row+i][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row+i,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col-i].isOccupied) break;
        }
        //right and up
        for(int i =1;i<8;i++){
            if((row-i) < 0 || (col+i) > 7) break;
            if(board[row-i][col+i].isOccupied){
                if(isWhite == board[row-i][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col+i].isOccupied) break;
        }
        //right and down
        for(int i=1;i<8;i++){
            if((row+i) > 7 || (col+i) >7) break;
            if(board[row+i][col+i].isOccupied){
                if(isWhite == board[row+i][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp={row+i, col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col+i].isOccupied) break;
        }
        //up
        for(int i =1;i<8;i++){
            if(row-i < 0) break;
            if(board[row-i][col].isOccupied){
                if(isWhite == board[row-i][col].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col};
            moves.insert({colToLetter.at(col)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col].isOccupied) break;
        }
        //down
        for(int i=1;i<8;i++){
            if(row+i > 7) break;
            if(board[row+i][col].isOccupied){
                if(isWhite == board[row+i][col].occupyingPiece->getIfWhite()) break;
            }
            temp = {row+i,col};
            moves.insert({colToLetter.at(col)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col].isOccupied) break;
        }
        //left
        for(int i=1;i<8;i++){
            if(col-i < 0) break;
            if(board[row][col-i].isOccupied){
                if(isWhite == board[row][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row),temp});
            if(board[row][col-i].isOccupied) break;
        }
        //right
        for(int i=1;i<8;i++){
            if(col+i > 7) break;
            if(board[row][col+i].isOccupied){
                if(isWhite == board[row][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row,col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row),temp});
            if(board[row][col+i].isOccupied) break;
        }
    }
    //Get possible location for king movement
    else if(thisPiece.name == "King"){
        for(int i = -1; i<2;i++){
            if(i+row >= 0 && i+row <8){
                for(int j = -1; j<2;j++){
                    if(j+col >= 0 && j+col <8){
                        if(!board[i+row][j+col].isOccupied) {
                            //temp = {i+row,j+col};
                            string temp1 = colToLetter.at(col+j);
                            string temp2 = rowToRowInt.at(row+i);
                            moves.insert({temp1+temp2,{i+row,j+col}});
                        }
                        else if(board[i+row][j+col].occupyingPiece->getIfWhite() != isWhite){
                             moves.insert({colToLetter.at(col+j)+rowToRowInt.at(row+i),{i+row,j+col}});
                        }
                    }
                }
            }
        }
        //Check if castling is possible
        if(!hasMoved && !inCheck){
            map<string,vector<int>>::iterator tmp;
            string rowNum;
            if(board[row][7].isOccupied && !board[row][7].occupyingPiece->getIfMoved()){
                //king side castle
                if(!board[row][5].isOccupied && !board[row][6].isOccupied){
                    rowNum = rowToRowInt.find(row)->second;
                    tmp = inCheckSpots.find("F"+rowNum);
                    if(tmp == inCheckSpots.end()) moves.insert({"G"+rowNum,{row,6}});
                }
            }
            if(board[row][0].isOccupied && !board[row][0].occupyingPiece->getIfMoved()){
                //queen side castle
                if(!board[row][3].isOccupied && !board[row][2].isOccupied && !board[row][1].isOccupied){
                    rowNum = rowToRowInt.find(row)->second;
                    tmp = inCheckSpots.find("D"+rowNum);
                    if(tmp == inCheckSpots.end()) moves.insert({"C"+rowNum,{row,2}});
                }
            }
        }
    }
    //Get possible location for Bishop movement
    else if(thisPiece.name == "Bishop"){
        //left and up
        for(int i=1;i<8;i++){
            if((row-i) < 0 || (col-i) < 0) break;
            if(board[row-i][col-i].isOccupied){
                if(isWhite == board[row-i][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col-i].isOccupied) break;
        }
        //left and down
        for(int i=1;i<8;i++){
            if((row+i) > 7 || (col-i) < 0) break;
            if(board[row+i][col-i].isOccupied){
                if(isWhite == board[row+i][col-i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row+i,col-i};
            moves.insert({colToLetter.at(col-i)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col-i].isOccupied) break;
        }
        //right and up
        for(int i =1;i<8;i++){
            if((row-i) < 0 || (col+i) > 7) break;
            if(board[row-i][col+i].isOccupied){
                if(isWhite == board[row-i][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp = {row-i,col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row-i),temp});
            if(board[row-i][col+i].isOccupied) break;
        }
        //right and down
        for(int i=1;i<8;i++){
            if((row+i) > 7 || (col+i) >7) break;
            if(board[row+i][col+i].isOccupied){
                if(isWhite == board[row+i][col+i].occupyingPiece->getIfWhite()) break;
            }
            temp={row+i, col+i};
            moves.insert({colToLetter.at(col+i)+rowToRowInt.at(row+i),temp});
            if(board[row+i][col+i].isOccupied) break;
        }
    }
    //Get possible location for Knight movement
    else if(thisPiece.name == "Knight"){
        bool up1 = true;
        bool up2 = true;
        bool down1 = true;
        bool down2 = true;
        bool left1 = true;
        bool left2 = true;
        bool right1 = true;
        bool right2 = true;

        if(row-1 < 0){
            up1 = false;
            up2 = false;
        }
        else{
            if(row-2 <0) left2 = false;
        }

        if(row+1 > 7){
            down1 = false;
            down2 = false;
        }
        else{
            if(row+2 >7) right2 = false;
        }

        if(col-1 <0){
            left1 = false;
            left2 = false;
        }
        else{
            if(col-2 < 0) left2 = false;
        }

        if(col+1 > 7){
            right1 = false;
            right2 = false;
        }
        else{
            if(col+2 > 7) right2 = false;
        }

        if(up2 && left1){
            if(board[row-2][col-1].isOccupied){
                if(isWhite != board[row-2][col-1].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row-2),{row-2,col-1}});
            }
            else{
                moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row-2),{row-2,col-1}});
            }
                    
        } 
        if(up2 && right1){
            if(board[row-2][col+1].isOccupied){
                if(isWhite != board[row-2][col+1].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row-2),{row-2,col+1}});
            }
            else{
                moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row-2),{row-2,col+1}});
            } 
        } 
            
        if(up1 && left2){
            if(board[row-1][col-2].isOccupied){
                if(isWhite != board[row-1][col-2].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col-2)+rowToRowInt.at(row-1),{row-1,col-2}});
            }
            else{
                moves.insert({colToLetter.at(col-2)+rowToRowInt.at(row-1),{row-1,col-2}});
            } 
        }

        if(up1 && right2){
            if(board[row-1][col+2].isOccupied){
                if(isWhite != board[row-1][col+2].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col+2)+rowToRowInt.at(row-1),{row-1,col+2}});
            }
            else{
                moves.insert({colToLetter.at(col+2)+rowToRowInt.at(row-1),{row-1,col+2}});
            }                 
        }
        if(down1 && left2){
            if(board[row+1][col-2].isOccupied){
                if(isWhite != board[row+1][col-2].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col-2)+rowToRowInt.at(row+1),{row+1,col-2}});
            }
            else{
                moves.insert({colToLetter.at(col-2)+rowToRowInt.at(row+1),{row+1,col-2}});
            } 
        } 

        if(down1 && right2){
            if(board[row+1][col+2].isOccupied){
                if(isWhite != board[row+1][col+2].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col+2)+rowToRowInt.at(row+1),{row+1,col+2}});
            }
            else{
                moves.insert({colToLetter.at(col+2)+rowToRowInt.at(row+1),{row+1,col+2}});
            }   
        } 
        if(down2 && left1){
            if(board[row+2][col-1].isOccupied){
                if(isWhite != board[row+2][col-1].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row+2),{row+2,col-1}});
            }
            else{
                moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row+2),{row+2,col-1}});
            }
        }
        if(down2 && right1){
            if(board[row+2][col+1].isOccupied){
                if(isWhite != board[row+2][col+1].occupyingPiece->getIfWhite()) moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row+2),{row+2,col+1}});
            }
            else{
                moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row+2),{row+2,col+1}});
            }
        }
    }
    //Get possible locations of Pawn movement
    else if(thisPiece.name == "Pawn"){
        int side = 1;
        if(isWhite) side = -1;
        //standard movement
        if(!board[row+side][col].isOccupied) moves.insert({colToLetter.at(col)+rowToRowInt.at(row+side),{row+side,col}});
        if(!hasMoved && !board[row+2*side][col].isOccupied) moves.insert({colToLetter.at(col)+rowToRowInt.at(row+2*side),{row+2*side,col}});
        //diagonal take
        //pawn on edge of board
        if(col == 0 || col ==7){
            if(col == 0 && board[row+side][col+1].isOccupied){
                if(board[row+side][col+1].occupyingPiece->getIfWhite() != isWhite) moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row+side),{row+side,col+1}});
            }
            if(col == 7 && board[row+side][col-1].isOccupied){
                if(board[row+side][col-1].occupyingPiece->getIfWhite() != isWhite) moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row+side),{row+side,col-1}});
            }
        }
        //pawn in middle of board
        else{
            if(board[row+side][col+1].isOccupied){
                if(board[row+side][col+1].occupyingPiece->getIfWhite() != isWhite) moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row+side),{row+side,col+1}});
            }
            if(board[row+side][col-1].isOccupied){
                if(board[row+side][col-1].occupyingPiece->getIfWhite() != isWhite) moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row+side),{row+side,col-1}});
            }
        }
        //en passant
        if(!history.empty()){
            string lastMove = history.back();
            string delimiter = " ";
            string lastPiece = lastMove.substr(0,lastMove.find(delimiter));
            lastMove.erase(0,lastMove.find(delimiter)+delimiter.length());
            string lastCol = lastMove.substr(0,1);
            string lastRow = lastMove.substr(1,1);
            map<string,int>::iterator colIdx = letterToCol.find(lastCol);
            map<string,int>::iterator rowIdx = rowIntToRow.find(lastRow);
            int lastColInt = colIdx->second;
            int lastRowInt = rowIdx->second;
            int reqRow = 3;
            if(!isWhite) reqRow = 4;
            bool movedTwice = false;
            for(int i=0;i<history.size()-1;i++){
                if(history[i].find(lastPiece) != string::npos) movedTwice = true;
            }      

            if(row == lastRowInt && row == reqRow){
                if(col == lastColInt+1 && !movedTwice){
                    moves.insert({colToLetter.at(col-1)+rowToRowInt.at(row+side),{row+side,col-1}});
                }
                if(col == lastColInt-1 && !movedTwice){
                    moves.insert({colToLetter.at(col+1)+rowToRowInt.at(row+side),{row+side,col+1}});                
                }
            }
        } 

    }
    return moves;
}

tuple<bool,map<string,vector<int>>> checkInCheck(map<string,chessPiece*>* _attackPieces, string kingSpot, Spot board[8][8], vector<string> history){
    /*This functions takes in all the information and determines if the King is in check
    It returns a boolean of true/false on if the king is/isn't in check as well as a map 
    of all the locations on the board that would result in the king being in check*/
    map<string,chessPiece*>::iterator it;
    map<string,vector<int>>::iterator itx;
    map<string,vector<int>> possibleMoves;
    map<string,vector<int>>::iterator itMove;
    map<string,vector<int>> allCheckSpots;
    map<string,vector<int>> tmp;
    bool inCheck = false;

    for(it = (*_attackPieces).begin();it!=(*_attackPieces).end();it++){
        possibleMoves = getPossibleSpots(board,*(it->second),history,false,tmp);
        for(itx=possibleMoves.begin(); itx != possibleMoves.end();itx++){
            allCheckSpots.insert({itx->first,itx->second});
        }
        itMove = possibleMoves.find(kingSpot);
        if(itMove != possibleMoves.end()) inCheck = true;
    }

    return {inCheck,allCheckSpots};
}

bool checkIfLegalMove(map<string,chessPiece*>* _map,string _piece, string _move, Spot board[8][8], string _currKing, map<string,chessPiece*>* _foePieces, vector<string> history,bool inCheck, map<string,vector<int>> checkSpots){
    //This function checks to see if a move input by the player is a legal move
    map<string,chessPiece*> ::iterator idx = (*_map).find(_piece);
    if(idx == (*_map).end()) return false;
    chessPiece thisPiece = *(idx->second);
    string toColumn = _move.substr(0,1);
    string toRow = _move.substr(1,1);
    map<string,vector<int>> possibleMoves = getPossibleSpots(board, thisPiece,history,inCheck,checkSpots);
    map<string,vector<int>> ::iterator moveIdx = possibleMoves.find(_move);
    map<int,string>::iterator kingIt;
    //If the player input move doesn't show up in the map of possible moves, return false
    if(moveIdx == possibleMoves.end()) return false;
    /*If the player input move does show up in the map of possible moves, make sure that if the movement is done
    it does not put the own player's king into check. To not put an official move on the board, a copy of the board
    gets made and the move is then checked to see if a check is put on the players king*/
    Spot testBoard[8][8];
    for(int i =0;i<8;i++){
        for(int j=0;j<8;j++){
            testBoard[i][j] = board[i][j];
        }
    }
    chessPiece temp = thisPiece;
    chessPiece * a;
    chessPiece **thisPieceP;
    a = &temp;
    thisPieceP = &a;
    map<string,int>::iterator colIdx = letterToCol.find(toColumn);
    map<string,int>::iterator rowIdx = rowIntToRow.find(toRow);
    map<string,chessPiece*>* tempFoePieces;
    map<string,chessPiece*>::iterator itx;
    map<string,chessPiece*> fakePieces;
    string foeColor = "W";
    if (temp.isWhite) foeColor = "B";
    for(itx = (*_foePieces).begin();itx != (*_foePieces).end();itx++){
        chessPiece* tempPiece = itx->second;
        fakePieces.insert({foeColor+itx->second->getShortName()+itx->second->getPieceNum(),tempPiece});
    }
    tempFoePieces = &fakePieces;
    int newCol = colIdx->second;
    int newRow = rowIdx->second;
    movePiece(testBoard,thisPieceP,newRow,newCol,tempFoePieces);
    int kingRowInt;
    int kingColInt;
    if(temp.name == "King"){
        kingRowInt = newRow;
        kingColInt = newCol;
    }
    else{
        idx = (*_map).find(_currKing);
        chessPiece king = *(idx->second);
        kingRowInt = king.current.row;
        kingColInt = king.current.col;
    }

    kingIt = colToLetter.find(kingColInt);
    string kingCol = kingIt->second;
    kingIt = rowToRowInt.find(kingRowInt);
    string kingRow = kingIt->second;
    string kingSpot = kingCol+kingRow;
    bool putSelfInCheck; map<string,vector<int>> tmp;
    tie(putSelfInCheck,tmp) = checkInCheck(_foePieces,kingSpot,testBoard, history);
    if(putSelfInCheck) return false;
    return true;
};


void displayBoard(Spot board[8][8]){
    //This function displays the board
    for (int i =0; i<8;i++){
        cout <<"\t---------------------------------\n";
        cout <<to_string(8-i)<<"\t|";
        for(int j=0; j<8;j++){
            if(board[i][j].isOccupied){
                if(board[i][j].occupyingPiece->getIfWhite()){
                    cout << "W" <<board[i][j].occupyingPiece->getShortName() <<board[i][j].occupyingPiece->getPieceNum();
                }
                else{
                    cout << "B" <<board[i][j].occupyingPiece->getShortName()<<board[i][j].occupyingPiece->getPieceNum();
                }
            }
            else{
                cout <<"   ";
            }
            cout <<"|";
        }
        cout <<"\n";
    }
    cout <<"\t---------------------------------\n";
    cout <<"\t  A   B   C   D   E   F   G   H\n";
}



int main(){
    int numPlayers;
    cout <<"\n\nLet's Play Chess!\nEnter moves as piece then move (ex WP1 A3)\nTo castle, identify as the king moving (ex WK1 C1)\n";
    Spot board[8][8];
    vector<string> moveHistory; 
    map<string,chessPiece*> whitePieces;
    map<string,chessPiece*> blackPieces;
    King King1(true,"1");
    King King2(false,"1");
    Queen Queen1(true,"1");
    Queen Queen2(false,"1");
    Bishop Bishop1(true,"1");
    Bishop Bishop2(true,"2");
    Bishop Bishop3(false,"1");
    Bishop Bishop4(false,"2");
    Knight Knight1(true,"1");
    Knight Knight2(true,"2");
    Knight Knight3(false,"1");
    Knight Knight4(false,"2");    
    Rook Rook1(true,"1");
    Rook Rook2(true,"2");
    Rook Rook3(false,"1");
    Rook Rook4(false,"2");
    Pawn Pawn1(true,"1");
    Pawn Pawn2(true,"2");
    Pawn Pawn3(true,"3");
    Pawn Pawn4(true,"4");
    Pawn Pawn5(true,"5");
    Pawn Pawn6(true,"6");
    Pawn Pawn7(true,"7");
    Pawn Pawn8(true,"8");
    Pawn Pawn9(false,"1");
    Pawn Pawn10(false,"2");
    Pawn Pawn11(false,"3");
    Pawn Pawn12(false,"4");
    Pawn Pawn13(false,"5");
    Pawn Pawn14(false,"6");
    Pawn Pawn15(false,"7");
    Pawn Pawn16(false,"8");
    board[0][0].setPiece(Rook3);
    Rook3.current = {0,0};
    blackPieces.insert({"BR1",&Rook3});
    board[0][1].setPiece(Knight3);
    Knight3.current = {0,1};
    blackPieces.insert({"BH1",&Knight3});
    board[0][2].setPiece(Bishop3);
    Bishop3.current = {0,2};
    blackPieces.insert({"BB1",&Bishop3});
    board[0][3].setPiece(Queen2);
    Queen2.current={0,3};
    blackPieces.insert({"BQ1",&Queen2});
    board[0][4].setPiece(King2);
    King2.current={0,4};
    blackPieces.insert({"BK1",&King2});
    board[0][5].setPiece(Bishop4);
    Bishop4.current = {0,5};
    blackPieces.insert({"BB2",&Bishop4});
    board[0][6].setPiece(Knight4);
    Knight4.current ={0,6};
    blackPieces.insert({"BH2",&Knight4});
    board[0][7].setPiece(Rook4);
    Rook4.current = {0,6};
    blackPieces.insert({"BR2",&Rook4});
    board[1][0].setPiece(Pawn9);
    Pawn9.current = {1,0};
    blackPieces.insert({"BP1",&Pawn9});
    board[1][1].setPiece(Pawn10);
    Pawn10.current = {1,1};
    blackPieces.insert({"BP2",&Pawn10});
    board[1][2].setPiece(Pawn11);
    Pawn11.current = {1,2};
    blackPieces.insert({"BP3",&Pawn11});
    board[1][3].setPiece(Pawn12);
    Pawn12.current = {1,3};
    blackPieces.insert({"BP4",&Pawn12});
    board[1][4].setPiece(Pawn13);
    Pawn13.current = {1,4};
    blackPieces.insert({"BP5",&Pawn13});
    board[1][5].setPiece(Pawn14);
    Pawn14.current = {1,5};
    blackPieces.insert({"BP6",&Pawn14});
    board[1][6].setPiece(Pawn15);
    Pawn15.current = {1,6};
    blackPieces.insert({"BP7",&Pawn15});
    board[1][7].setPiece(Pawn16);
    Pawn16.current = {1,7};
    blackPieces.insert({"BP8",&Pawn16});    

    board[7][0].setPiece(Rook1);
    Rook1.current = {7,0};
    whitePieces.insert({"WR1",&Rook1});
    board[7][1].setPiece(Knight1);
    Knight1.current = {7,1};
    whitePieces.insert({"WH1",&Knight1});
    board[7][2].setPiece(Bishop1);
    Bishop1.current = {7,2};
    whitePieces.insert({"WB1",&Bishop1});
    board[7][3].setPiece(Queen1);
    Queen1.current={7,3};
    whitePieces.insert({"WQ1",&Queen1});
    board[7][4].setPiece(King1);
    King1.current={7,4};
    whitePieces.insert({"WK1",&King1});
    board[7][5].setPiece(Bishop2);
    Bishop2.current = {7,5};
    whitePieces.insert({"WB2",&Bishop2});
    board[7][6].setPiece(Knight2);
    Knight2.current ={7,6};
    whitePieces.insert({"WH2",&Knight2});
    board[7][7].setPiece(Rook2);
    Rook2.current = {7,6};
    whitePieces.insert({"WR2",&Rook2});
    board[6][0].setPiece(Pawn1);
    Pawn1.current = {6,0};
    whitePieces.insert({"WP1",&Pawn1});
    board[6][1].setPiece(Pawn2);
    Pawn2.current = {6,1};
    whitePieces.insert({"WP2",&Pawn2});
    board[6][2].setPiece(Pawn3);
    Pawn3.current = {6,2};
    whitePieces.insert({"WP3",&Pawn3});
    board[6][3].setPiece(Pawn4);
    Pawn4.current = {6,3};
    whitePieces.insert({"WP4",&Pawn4});
    board[6][4].setPiece(Pawn5);
    Pawn5.current = {6,4};
    whitePieces.insert({"WP5",&Pawn5});
    board[6][5].setPiece(Pawn6);
    Pawn6.current = {6,5};
    whitePieces.insert({"WP6",&Pawn6});
    board[6][6].setPiece(Pawn7);
    Pawn7.current = {6,6};
    whitePieces.insert({"WP7",&Pawn7});
    board[6][7].setPiece(Pawn8);
    Pawn8.current = {6,7};
    whitePieces.insert({"WP8",&Pawn8});    

    displayBoard(board);
    bool whiteTurn = false;
    bool checkMate = false;
    bool inCheck = false;
    
    map<string,chessPiece*>* thisTurnsPieces;
    map<string,chessPiece*>* foePieces;
    string move;
    string piece;
    string playerNum;
    string oppKing;
    string currKing;
    int promoCount;
    chessPiece promoPieces[16];
    map<string,chessPiece*>::iterator idx;
    map<int,string>::iterator kingIt;
    chessPiece* thisPieceA;
    chessPiece **thisPiece;
    map<string,vector<int>> checkSpots;
    while(!checkMate){
        whiteTurn = !whiteTurn;
        if(whiteTurn){
            playerNum = "Player 1";
            currKing = "WK1";
            oppKing = "BK1";
        }
        else{
            playerNum = "Player 2";
            currKing = "BK1";
            oppKing = "WK1";
        }
        cout<< playerNum << " Enter a Move:   ";
        cin >> piece >> move;
        if(whiteTurn){
            thisTurnsPieces = &whitePieces;
            foePieces = &blackPieces;
        }
        else{
            thisTurnsPieces = &blackPieces;
            foePieces = &whitePieces;
        }
        bool legalMove = checkIfLegalMove(thisTurnsPieces,piece,move,board,currKing,foePieces,moveHistory,inCheck,checkSpots);
        if(legalMove){
            inCheck = false;
            idx = (*thisTurnsPieces).find(piece);
            thisPieceA = idx->second;
            thisPiece = &thisPieceA;
            string toColumn = move.substr(0,1);
            string toRow = move.substr(1,1);
            map<string,int>::iterator colIdx = letterToCol.find(toColumn);
            map<string,int>::iterator rowIdx = rowIntToRow.find(toRow);
            int newCol = colIdx->second;
            int newRow = rowIdx->second;
            movePiece(board, thisPiece, newRow, newCol, foePieces);
            moveHistory.push_back(piece+" "+move);
            if((*thisPiece)->getPiece() == "Pawn" && (newRow == 0 || newRow == 7)){
                cout << "What piece would you like to get promoted to? Options are Queen, Knight, Rook, or Bishop:   ";
                string newPiece;
                cin >> newPiece;
                map<string,chessPiece*>::iterator proItx;
                map<string,int> numPieces;
                for(proItx = (*thisTurnsPieces).begin();proItx != (*thisTurnsPieces).end();proItx++){
                    chessPiece temp = *(proItx->second);
                    numPieces[temp.name] = numPieces[temp.name] +1;
                }
                string color = "W";
                if(!whiteTurn) color = "B";
                
                if(newPiece == "Queen"){
                    Queen foo(whiteTurn,to_string(numPieces["Queen"]+1));
                    promoPieces[promoCount] = foo;
                    promoPieces[promoCount].current.col = newCol;
                    promoPieces[promoCount].current.row = newRow;
                    (*thisTurnsPieces).insert({color+"Q"+to_string(numPieces["Queen"]+1),&promoPieces[promoCount]});
                    board[newRow][newCol].setPiece(promoPieces[promoCount]);                
                }
                else if(newPiece == "Bishop"){
                    Bishop foo(whiteTurn,to_string(numPieces["Bishop"]+1));
                    promoPieces[promoCount] = foo;
                    promoPieces[promoCount].current.col = newCol;
                    promoPieces[promoCount].current.row = newRow;
                    (*thisTurnsPieces).insert({color+"B"+to_string(numPieces["Bishop"]+1),&promoPieces[promoCount]});
                    board[newRow][newCol].setPiece(promoPieces[promoCount]);               
                }
                else if(newPiece == "Knight"){
                    Knight foo(whiteTurn,to_string(numPieces["Knight"]+1));
                    promoPieces[promoCount] = foo;
                    promoPieces[promoCount].current.col = newCol;
                    promoPieces[promoCount].current.row = newRow;
                    (*thisTurnsPieces).insert({color+"H"+to_string(numPieces["Knight"]+1),&promoPieces[promoCount]});
                    board[newRow][newCol].setPiece(promoPieces[promoCount]);             
                }
                else if(newPiece == "Rook"){
                    Rook foo(whiteTurn,to_string(numPieces["Rook"]+1));
                    promoPieces[promoCount] = foo;
                    promoPieces[promoCount].current.col = newCol;
                    promoPieces[promoCount].current.row = newRow;
                    (*thisTurnsPieces).insert({color+"R"+to_string(numPieces["Rook"]+1),&promoPieces[promoCount]});
                    board[newRow][newCol].setPiece(promoPieces[promoCount]);                    
                }
                promoCount++;
                (*thisTurnsPieces).erase(color+"P"+(*thisPiece)->getPieceNum());
            }
            thisPiece = nullptr;
            //check to see if foe's king is in check
            map<string,chessPiece*>::iterator it = (*foePieces).find(oppKing);
            chessPiece foeKing = *(it->second);
            string foeKingSpot;
            int foeKingRowInt = foeKing.current.row;
            int foeKingColInt = foeKing.current.col;
            kingIt = colToLetter.find(foeKingColInt);
            string foeKingCol = kingIt->second;
            kingIt = rowToRowInt.find(foeKingRowInt);
            string foeKingRow = kingIt->second;
            foeKingSpot = foeKingCol+foeKingRow;
            bool inCheck; checkSpots;
            tie(inCheck,checkSpots) = checkInCheck(thisTurnsPieces,foeKingSpot,board, moveHistory);
            if(inCheck) cout <<"CHECK!";
        }
        else{
            cout <<"This is an illegal move try again\n";
            whiteTurn = !whiteTurn;
        }
        //check to see if foe's king is in checkmate now
        if(inCheck){
            map<string,vector<int>>::iterator mateIt;
            map<string,chessPiece*>::iterator itx;

            bool singleViableMove = false;
            for(itx = (*foePieces).begin();itx != (*foePieces).end();itx++){
                map<string,vector<int>> possibleMoves = getPossibleSpots(board, *(itx->second),moveHistory,inCheck,checkSpots);
                for(mateIt=possibleMoves.begin();mateIt != possibleMoves.end();mateIt++){
                    singleViableMove = checkIfLegalMove(foePieces,itx->first,mateIt->first,board,oppKing,thisTurnsPieces,moveHistory,inCheck,checkSpots);
                    if(singleViableMove) break;
                }
                if(singleViableMove) break;
            }
            if(!singleViableMove) checkMate = true;
        }
        displayBoard(board);
    }
    cout <<"CHECKMATE";

    return 0;
}