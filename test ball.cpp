
#include "lib/framework.hpp"
#include <iostream>
#include <string>
using namespace std;

#define ball_num 50

enum WindowSize{
	width = 1280,
	height = 720,
};

//ベクトル間の二点間距離(float)
//使ってない
float dist(Vec2f a, Vec2f b){
	float c = sqrt(b.x() - a.x())*(b.x() - a.x()) + (b.y() - a.y())*(b.y() - a.y());
	return c;
}

//ベクトル間の二点間距離(Vec2f)
//使ってない
Vec2f Vec2f_get_dist(Vec2f a, Vec2f b){
	Vec2f c;
	c.x() = (b.x() - a.x());
	c.y() = (b.y() - a.y());
	return c;
}

//ベクトルの長さを計算する
float get_dist(Vec2f v){
	return pow((v.x() * v.x()) + (v.y() * v.y()), 0.5);
}

//単位ベクトルを計算する
Vec2f get_unit(Vec2f v){
	//ベクトルの長さ
	float length = get_dist(v);

	//x,y各成分を長さで割る
	Vec2f unit;
	unit.x() = v.x() / length;
	unit.y() = v.y() / length;

	return unit;
}

//ベクトル内積
float dot_product(Vec2f vl, Vec2f vr){
	return vl.x() * vr.x() + vl.y() * vr.y();
}

//2つのベクトルのなす角θを求める
//結局使ってない
float angle_vector(Vec2f a, Vec2f b){
	float len_a = get_dist(a);
	float len_b = get_dist(b);

	float cos_th = dot_product(a, b) / (len_a * len_b);

	float theta = acos(cos_th);

	//theta = theta * 180.0f / M_PI;

	return theta;
}

//円同士の衝突時の法線ベクトル
Vec2f get_normal(Vec2f vl, Vec2f vr, float vs){
	vr -= vl;
	Vec2f normal = (vr / -2.0f)/ vs/*ボールの大きさで割ったらちょうどよかったので*/;

	return normal;
}

//円同士の反射ベクトル
Vec2f get_ref(Vec2f a, Vec2f vl, Vec2f vr, float vs){
	Vec2f normal = get_normal(vl, vr, vs);
	Vec2f reflection = a + 2 * dot_product(-a, normal) * normal;

	return reflection;
}

int main() {

	AppEnv env(width, height, false, true);

	Random rand;
	rand.setSeed(time(nullptr));

	struct Ball{
		Vec2f pos;
		Vec2f speed;
		float size;
	}ball[ball_num];

	float g = -9.8 / 60;

	for (int i = 0; i < ball_num; ++i){
		ball[i].pos = Vec2f(rand(-width / 2 + ball[i].size * 2, width / 2 - +ball[i].size * 2),
							rand(0, height/2));
		ball[i].speed = Vec2f(rand(-4.0f, 4.0f), 0);
		ball[i].size = 10;
	}

	for (;;) {
		if (!env.isOpen()) return 0;
		env.begin();

		for (int i = 0; i < ball_num; ++i){

			if (ball[i].pos.y() < -height / 2 + ball[i].size){
				ball[i].pos.y() = -height / 2 + ball[i].size;
				ball[i].speed.y() *= -1.0f;
				//ball[i].speed.x() *= 0.98f;

			}
			if (ball[i].pos.x() < -width / 2 + ball[i].size){
				ball[i].pos.x() = -width / 2 + ball[i].size;
				ball[i].speed.x() *= -1.0f;
			}
			if (ball[i].pos.x() > width / 2 - ball[i].size){
				ball[i].pos.x() = width / 2 - ball[i].size;
				ball[i].speed.x() *= -1.0f;
			}


			ball[i].speed.y() += g;

			ball[i].pos += ball[i].speed;

			drawFillCircle(ball[i].pos.x(), ball[i].pos.y(),
						   ball[i].size, ball[i].size,
						   100,
						   Color::white);


			for (int j = 0; j < ball_num; ++j){

				float a;
				float b;
				float c;
				float len;
				a = (ball[i].pos.x() - ball[j].pos.x());
				b = (ball[i].pos.y() - ball[j].pos.y());
				c = sqrt(a * a + b * b);
				len = 2 * ball[i].size - c;

				//衝突して埋まった分、ボール同士を引き離す
				if (c < ball[i].size * 2){
					if (c > 0) c = 1 / c;
					a *= c;
					b *= c;
					len /= 2.0f;

					ball[i].pos.x() += a * len;
					ball[i].pos.y() += b * len;
					ball[j].pos.x() -= a * len;
					ball[j].pos.y() -= b * len;

					ball[i].speed = (get_ref(ball[i].speed, ball[i].pos, ball[j].pos, ball[i].size));
					//ball[j].speed = (get_ref(ball[j].speed, ball[j].pos, ball[i].pos, ball[j].size));
				}
			}
		}

		env.end();
	}
}
