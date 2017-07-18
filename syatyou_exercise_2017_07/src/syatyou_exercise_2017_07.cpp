/* 演習 社長からの課題
 * 「vCard形式」ファイルを修正するプログラム
 *
 */

#include <iostream>
#include <fstream>
#include <cctype>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <string>

using namespace std;

int main(int args, char** argv) {


	char* p = argv[1];
	string s = argv[2];
	ifstream ifs(p);
	ofstream ofs(s);

	if(!ifs) {
		cout << "オープン失敗";
	}
	if(!ofs) {
		cout << "オープン失敗";
	}

	string buff;				//一行ずつ読みこんでいくためのバッファ
	string addressData[50];		//フォーマット通りBEGINからENDまでのデータを格納
	string companyName;			//ORGから抽出した社名を格納する変数
	string firstname;			//X-PHONETIC-FIRST-NAME:から抽出した名前を格納する変数
	string lastname;			//X-PHONETIC-LAST-NAME:から抽出した名前を格納する変数
	string companySpell;		//NOTE:から抽出した社名の読みを格納する変数

	int FORMAT_NAME = 3;			//変更する名前のデータを格納してある場所
	int FORMAT_SORT = 4;			//変更するソートのデータを格納してある場所
	int FORMAT_LASTNAME = 6;		//変更する名前のデータを格納してある場所
	int ERROR_firstname = 0;

	//何人目かの確認用
	int no = 0;

	//addressDataの格納するインデックスに使う変数
	int i = 0;

	//最後の行まで繰り返す
	while(getline(ifs, buff)) {

		//名前の読みが始まる前にソートの読みの分を開けておく
		if(buff.find("X-PHONETIC-FIRST-NAME:") != string::npos) {
			addressData[i] += "SORT-STRING:";
			i++;
		}

		if(buff.find("X-PHONETIC-LAST-NAME:") != string::npos) {
			if(addressData[i - 1].find("X-PHONETIC-FIRST-NAME:") == string::npos) {
				addressData[i] +=  "SORT-STRING:";
				i++;
				FORMAT_LASTNAME--;
				ERROR_firstname = 1;
			}
		}

		//読みとったデータを格納
		addressData[i] = buff;

		//ORG:だったら会社名を取得
		if( buff.find("ORG:",0) != string::npos) {
			string::size_type start = 0;
			string::size_type t = 0;
			string::size_type number = 4;

			//ORG:分削除する
			buff.erase(start,number);

			//株式会社が残ってしまっている限り繰り返して取り除く
			while((t = buff.rfind("株式会社")) != string::npos) {
				//
				number = 12;
				buff.erase(t,number);
			}

			//有限会社が残ってしまっている限り繰り返して取り除く
			while((t = buff.rfind("有限会社")) != string::npos) {
				//
				number = 12;
				buff.erase(t,number);
			}

			companyName = buff;
		}

		//NOTE:だったら会社名の読みを取得
		if(buff.find("NOTE:",0) != string::npos) {
			int start;
			if((start = buff.find("社名読み", 0) + 14) != 13) {
				buff.erase(0, start);

				unsigned int mojinumber;
				char spelltemp[256] = {0};
				if((mojinumber = buff.find("\\n")) != string::npos) {
					buff.copy(spelltemp, mojinumber, 0);
					companySpell = spelltemp;
				}
			}
		}

		//名前の読みを取得
		if(buff.find("X-PHONETIC-FIRST-NAME:",0) != string::npos) {
			char firstnametemp[30] = "";
			buff.copy(firstnametemp, 256, 22);
			firstname = firstnametemp;
		}

		//苗字の読みを取得
		if(buff.find("X-PHONETIC-LAST-NAME:",0) != string::npos) {
			char lastnametemp[31] = "";
			buff.copy(lastnametemp, 256, 21);
			lastname = lastnametemp;
		}

		//最後まで来た時
		if(addressData[i].find("END:VCARD",0) != string::npos) {

			//仕様に合わせて文字列を付け足す
			companyName += "）";
			companySpell += "）";
			firstname += " ";

			//名前に会社名追加
			addressData[FORMAT_NAME].insert(2, companyName);

			//名前読みに会社読みを追加
			addressData[FORMAT_LASTNAME].insert(21, companySpell);

			string temp = companySpell  + firstname + lastname;
			//ソートに会社読み、名前、苗字を追加する
			addressData[FORMAT_SORT] += temp;

			//出来上がったBEGIN(0)からEND(i番目)までをファイルに抽出
			for(int j = 0; j <= i; j++) {
				ofs << addressData[j] << endl;
			}

			//格納した全データをクリア
			for(int j = 0; j < 50; j++) {
				addressData[j].clear();
			}

			if(ERROR_firstname == 1) {
				FORMAT_LASTNAME++;
				ERROR_firstname = 0;
			}

			//名前などに使った各文字列をクリア
			companyName.clear();
			companySpell.clear();
			firstname.clear();
			lastname.clear();

			//次の連絡先に進むために0からにするがこの後インクリメントがあるため-1でクリア
			i = -1;
			no++;
		}

		//次のデータに進む
		i++;

		//一行ごとに読みこんだデータをクリア
		buff.clear();
	}

	return 0;


}
