# include <Siv3D.hpp> // OpenSiv3D v0.6.11d

struct Unit
{
	double x;
	double y;
	int HP;
	int co;		//当たり判定の大きさ(円)
	bool enable;//有効フラグ
	int inv;	//無敵時間
	int exp;	//経験値
	int level;	//レベル

	double vx;	//x軸の移動量
	double vy;	//y軸の移動量
};
struct Weapon
{
	double x;
	double y;
	double vx;
	double vy;
	int co;		//当たり判定の大きさ(円)
	int atk;	//武器の攻撃力
	bool enable;//有効フラグ
	int level;	//武器レベル
	int wm;		//武器の向き
};

const int slimenum = 100;	//スライムの最大数
const int knifenum = 100;	//ナイフの最大数

Unit abe;
Unit slime[slimenum];
Unit dia[ slimenum ];
Weapon knife[knifenum];
Weapon katana;

void init();	//初期化設定
void player();	//自機
void weapon();	//武器
void Knife();	//ナイフ
void Katana();	//刀
void enemy();	//敵機
void Levelup();	//レベルアップ時の演出
void draw();	//描画
void direction();//自機の向き

bool collision(Unit a, Unit b);	//当たり判定関数
bool weaponcollsion(Weapon a, Unit b);	//武器の当たり判定関数

int muki = 0;	//自機の向き
int wct = 0;	//武器のクールタイム
int delay = 0;	//斜めに向いたときディレイをかける
int wno = 0;	//武器のナンバー

bool pause = false;		//ポーズ機能
bool levelup = false;	//レベルアップ演出
int select = 0;			//レベルアップボーナス選択用


void Main()
{

	init();
	//Scene::SetBackground(ColorF{ 0.3, 1.0, 0.3 });

	//アセットを登録する
	TextureAsset::Register(U"map", U"map.png");
	TextureAsset::Register(U"abeimg",U"abe.png");
	TextureAsset::Register(U"knifeimg", U"ナイフ.png");
	TextureAsset::Register(U"knifeicon", U"knifeicon.png");
	TextureAsset::Register(U"katanaimg", U"刀.png");
	TextureAsset::Register(U"katanaicon", U"刀アイコン.jpg");
	TextureAsset::Register(U"slimeimg", U"slime.png");
	TextureAsset::Register(U"diaimg", U"diamond.png");

	FontAsset::Register(U"Reggae One",20,U"Reggae-master/fonts/ttf/ReggaeOne-Regular.ttf");



	while (System::Update())
	{
		if (pause == false && levelup == false)
		{
			direction();
			player();
			enemy();
			weapon();
		}
		draw();

		if (levelup == true&& pause == false)
		{
			Levelup();
		}

		if (KeyEnter.down())
		{
			if (pause == true)
			{
				pause = false;
			}
			else if (pause == false)
			{
				pause = true;
			}
		}
	}

}

//変数の初期化
void init()
{
	abe.x = 400;
	abe.y = 300;
	abe.HP = 1000;
	abe.co = 20;
	abe.level = 1;
	abe.exp = 0;
	abe.enable = true;

	for (int i = 0; i < slimenum; i++)
	{
		slime[i].x = 0;
		slime[i].y = 0;
		slime[i].vx = 0;
		slime[i].vy = 0;
		slime[i].HP = 30;
		slime[i].co = 10;
		slime[i].enable = false;
	}

	for (int i = 0; i < knifenum; i++)
	{
		knife[i].level = 1;
		knife[i].enable = false;
		knife[i].co = 5;
		knife[i].atk = 10;
	}

	katana.level = 0;
	katana.enable = false;
	katana.co = 20;
	katana.atk = 30;

	for (int i = 0; i < slimenum; i++)
	{
		dia[i].co = 10;
		dia[i].enable = false;
	}
}

//自機のアップデート関数
void player()
{

	if (KeyD.pressed() && abe.x <= 790)
	{
		++abe.x;
	}
	if (KeyA.pressed() && abe.x > 10)
	{
		--abe.x;
	}
	if (KeyS.pressed() && abe.y <= 590)
	{
		++abe.y;
	}
	if (KeyW.pressed() && abe.y > 100)
	{
		--abe.y;
	}

	if (abe.HP <= 0)
	{
		abe.enable = false;
	}

	for (int i = 0; i < slimenum; i++)
	{
		
		if (collision(abe, dia[i]) && dia[i].enable == true)
		{
			abe.exp += 200;
			dia[i].enable = false;
		}
	}
	if (abe.exp >= 600)
	{
		levelup = true;
		++abe.level;
		abe.exp = 0;
	}

}

//敵機のアップデート関数
void enemy()
{
	for (int i = 0; i < slimenum; i++)
	{
		if (slime[i].enable == true)
		{
			double speed = 0.3;//速度
			double dx = abe.x - slime[i].x;//プレイヤーと敵のx方向の距離
			double dy = abe.y - slime[i].y;//プレイヤーと敵のy方向の距離
			double d = sqrt(dx * dx + dy * dy);//敵とプレイヤーとの距離
			slime[i].vx = speed * (dx / d);//xの移動量
			slime[i].vy = speed * (dy / d);//yの移動量

			slime[i].x += slime[i].vx;
			slime[i].y += slime[i].vy;

			if (collision(abe, slime[i]))
			{
				--abe.HP;
			}

			for (int j = 0; j < knifenum; j++)
			{
				if (weaponcollsion(knife[j], slime[i]) && knife[j].enable == true)
				{
					knife[j].vx = 0;
					knife[j].vy = 0;
					knife[j].enable = false;
					slime[i].HP -= knife[j].atk;
				}
			}

			if (slime[i].HP <= 0)
			{
				for (int j = 0; j < slimenum; j++)
				{
					if (dia[j].enable == false)
					{
						dia[j].x = slime[i].x;
						dia[j].y = slime[i].y;
						dia[j].enable = true;
						break;
					}
				}

				slime[i].enable = false;
			}
		}
		if (RandomBool(0.0001) == true&&slime[i].enable==false)
		{
			slime[i].x = Random(-100,900);
			slime[i].y = Random(-100,700);
			slime[i].HP = 30;
			slime[i].enable = true;
		}
		
	}
}

//武器のアップデート関数
void weapon()
{

	if (KeySpace.down())
	{
		wno++;
	}

	if (wno == 0)
	{
	Knife();
	}
	if (wno == 1)
	{
		Katana();
	}
	
	if (wct > 0)
	{
		--wct;
	}

}

//描画関数
void draw()
{

	//画像の描画
	TextureAsset(U"map").draw(0, 0);
	if (abe.enable == true)
	{
		//Circle{ abe.x,abe.y,abe.co }.draw(ColorF{ 1.0,1.0,1.0 });
		TextureAsset(U"abeimg").draw(abe.x - 15, abe.y - 20);
	}
	for (int i = 0; i < slimenum; i++)
	{
		if (slime[i].enable == true)
		{
			//Circle{ slime[i].x,slime[i].y,slime[i].co }.draw(ColorF{ 1.0,0.0,0.0 });

			if (abe.x <= slime[i].x)
			{
				TextureAsset(U"slimeimg").draw(slime[i].x - 20, slime[i].y - 30);
			}
			if (abe.x > slime[i].x)
			{
				TextureAsset(U"slimeimg").mirrored().draw(slime[i].x - 20, slime[i].y - 30);
			}
		}

		if (dia[i].enable == true)
		{
			//Circle{ dia[i].x,dia[i].y,dia[i].co }.draw(ColorF{ 0.0,1.0,0.0 });
			TextureAsset(U"diaimg").draw(dia[i].x - 13, dia[i].y - 13);
		}
	}

	for (int i = 0; i < knifenum; i++)
	{
		if (knife[i].enable == true)
		{
			//Circle{ knife[i].x,knife[i].y,knife[i].co }.draw(ColorF{ 0.0,1.0,0.0 });

			if (knife[i].wm == 0)
			{
				TextureAsset(U"knifeimg").rotated(135_deg).drawAt(knife[i].x - 10, knife[i].y);
			}
			if (knife[i].wm == 1)
			{
				TextureAsset(U"knifeimg").rotated(315_deg).draw(knife[i].x, knife[i].y-12);
			}
			if (knife[i].wm == 2)
			{
				TextureAsset(U"knifeimg").rotated(225_deg).draw(knife[i].x-12, knife[i].y-20);
			}
			if (knife[i].wm == 3)
			{
				TextureAsset(U"knifeimg").rotated(45_deg).draw(knife[i].x-12, knife[i].y);
			}
			if (knife[i].wm == 4)
			{
				TextureAsset(U"knifeimg").rotated(90_deg).draw(knife[i].x-20, knife[i].y-4);
			}
			if (knife[i].wm == 5)
			{
				TextureAsset(U"knifeimg").rotated(180_deg).draw(knife[i].x-20, knife[i].y-20);
			}
			if (knife[i].wm == 6)
			{
				TextureAsset(U"knifeimg").rotated(270_deg).draw(knife[i].x-3, knife[i].y-20);
			}
			if (knife[i].wm == 7)
			{
				TextureAsset(U"knifeimg").draw(knife[i].x-3, knife[i].y-3);
			}

		}
	}

	if (katana.enable == true)
	{
		Circle{ katana.x,katana.y,katana.co }.draw(ColorF{ 0.0,1.0,0.0 });
	}

	//自機のHPゲージ
	RectF{ abe.x-20,abe.y+20,abe.HP / 25,10 }.draw(ColorF{ 0.0,0.0,1.0 });

	//expゲージ
	Rect{ 100, 30, 600, 30 }.shearedX(10).draw(ColorF{ 0.0,0.0,0.0 });
	Rect{ 100, 30, abe.exp, 30 }.shearedX(10).draw(ColorF{ 0.3,1.0,0.3 });
	Rect{ 100, 30, 600, 30 }.shearedX(10).drawFrame(2);
	FontAsset(U"Reggae One")(U"Level.{}"_fmt(abe.level)).drawAt({ 150, 15 },ColorF{0.0,0.0,0.0});
	if (wno == 0)
	{
		TextureAsset(U"knifeicon").draw(10, 0);
	}
	if (wno == 1)
	{
		TextureAsset(U"katanaicon").draw(10, 0);
	}

	if (levelup == true)
	{
		if (select != 0)
		{
			RoundRect{ 100, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.7,0.0 });
		}
		else if (select == 0)
		{
			RoundRect{ 100, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.0,0.7 });
		}
		if (select != 1)
		{
			RoundRect{ 310, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.7,0.0 });
		}
		else if (select == 1)
		{
			RoundRect{ 310, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.0,0.7 });
		}
		if (select != 2)
		{
			RoundRect{ 520, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.7,0.0 });
		}
		else if (select == 2)
		{
			RoundRect{ 520, 200, 180, 225, 20 }.drawShadow(Vec2{ 2, 2 }, 8, 0).draw(ColorF{ 0.0,0.0,0.7 });
		}

	}

	if (pause == true)
	{
		Rect{ 0, 0, 800, 600 }.draw(ColorF{0.0,0.0,0.0,0.3});
	}

}

//ナイフのアップデート関数
void Knife()
{
	if (wct == 0)
	{
		//弾が無効なときのみ初期値をセットし有効にする
		for (int i = 0; i < knifenum; i++)
		{
			//撃てる弾をみつける
			if (knife[i].enable == false) {
				//弾を撃つ

				knife[i].wm = muki;

				if (knife[i].wm == 0)
				{
					knife[i].vx = 2;
				}
				if (knife[i].wm == 1)
				{
					knife[i].vx = -2;
				}
				if (knife[i].wm == 2)
				{
					knife[i].vy = 2;
				}
				if (knife[i].wm == 3)
				{
					knife[i].vy = -2;
				}
				if (knife[i].wm == 4)
				{
					knife[i].vx = 1.4;
					knife[i].vy = -1.4;
				}
				if (knife[i].wm == 5)
				{
					knife[i].vx = 1.4;
					knife[i].vy = 1.4;
				}
				if (knife[i].wm == 6)
				{
					knife[i].vx = -1.4;
					knife[i].vy = 1.4;
				}
				if (knife[i].wm == 7)
				{
					knife[i].vx = -1.4;
					knife[i].vy = -1.4;
				}

				knife[i].x = abe.x;
				knife[i].y = abe.y;

				knife[i].enable = true;
				wct = 50;//連射速度　小さいほど連射できる
				break;
			}
		}
	}
	//弾が有効なときに弾を動かす
	for (int i = 0; i < knifenum; i++)
	{
		if (knife[i].enable == true) {
			knife[i].x = knife[i].x + knife[i].vx;
			knife[i].y = knife[i].y + knife[i].vy;
			if (knife[i].x >= 800 || knife[i].y >= 600 || knife[i].x <= 0 || knife[i].y <= 0)
			{
				knife[i].vx = 0;
				knife[i].vy = 0;
				knife[i].enable = false;
			}
		}
	}
}

void Katana()
{
	if (wct == 0)
	{
		katana.x = abe.x;
		katana.y = abe.y;
		katana.enable = true;
	}
}

//レベルアップ時の関数
void Levelup()
{
	if (KeyD.down() && select != 2)
	{
		++select;
	}
	if (KeyA.down() && select != 0)
	{
		--select;
	}
	if (KeyShift.down())
	{
		levelup = false;
	}
}

//当たり判定関数
bool collision(Unit a, Unit b)
{
	const Circle A{ a.x,a.y,a.co };
	const Circle B{ b.x,b.y,b.co };

	if (A.intersects(B) && a.enable == true && b.enable == true)
	{
		//当たっている
		return true;
	}
	return false;
}
//武器の当たり判定関数
bool weaponcollsion(Weapon a, Unit b)
{
	const Circle A{ a.x,a.y,a.co };
	const Circle B{ b.x,b.y,b.co };

	if (A.intersects(B) && a.enable == true && b.enable == true)
	{
		//当たっている
		return true;
	}
	return false;
}

//向きの関数
void direction()
{
	//自機の向きを変える
	if (KeyD.pressed() && delay == 0)
	{
		muki = 0;
	}
	if (KeyA.pressed() && delay == 0)
	{
		muki = 1;
	}
	if (KeyS.pressed() && delay == 0)
	{
		muki = 2;
	}
	if (KeyW.pressed() && delay == 0)
	{
		muki = 3;
	}
	if (KeyW.pressed() && KeyD.pressed())
	{
		muki = 4;
		delay = 10;
	}
	if (KeyS.pressed() && KeyD.pressed())
	{
		muki = 5;
		delay = 10;
	}
	if (KeyS.pressed() && KeyA.pressed())
	{
		muki = 6;
		delay = 10;
	}
	if (KeyA.pressed() && KeyW.pressed())
	{
		muki = 7;
		delay = 10;
	}

	if (delay > 0)
	{
		--delay;
	}
}

