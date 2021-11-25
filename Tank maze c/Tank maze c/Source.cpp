#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _USE_MATH_DEFINES
#define NOMINMAX
#pragma comment(lib, "ws2_32.lib")
#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<unordered_map>
#include<Windows.h>
#include<algorithm>
#include<iostream>
#include<sstream>
#include<fstream>
#include<thread>
#include<random>
#include<chrono>
#include<thread>
#include<deque>
#include<cmath>
#include<mutex>
#include<deque>
#include<queue>
#include<map>
#include<set>
#define PACKETS_PER_SECOND 40
#define MAX_PLAYER_COUNT 10
#define PACKET_STORAGE_SIZE 100
#define DISCONNECT_AFTER 10
typedef double ld;
ld gettime() {
	return std::chrono::high_resolution_clock::now().time_since_epoch().count() / (long double)1e9;
}
std::mt19937 rnd(std::chrono::high_resolution_clock::now().time_since_epoch().count());
//std::mt19937 rnd(123);
long long randnum(long long l, long long r) {
	return l + rnd() % (r - l + 1);
}
using namespace sf;
#define eps 1e-7
ld len(ld a, ld b) {
	return sqrtl(a * a + b * b);
}
struct point {
	ld x, y;
	point() {}
	point(ld _x, ld _y) :x(_x), y(_y) {}
	ld len2() {
		return x * x + y * y;
	}
	ld len() {
		return sqrt(len2());
	}
};
struct line {
	ld a, b, c;
	line() {}
	line(ld _a, ld _b, ld _c) :a(_a), b(_b), c(_c) {}
	line(const point& x, const point& v) {
		a = -v.y;
		b = v.x;
		c = -(a * x.x + b * x.y);
	}
	ld f(point p)const {
		return a * p.x + b * p.y + c;
	}
	ld gety(ld x)const {
		return (a * x + c) / -b;
	}
	ld getx(ld y)const {
		return (b * y + c) / -a;
	}
};
bool doIntersect(const line& l, const point& a, const point& b) {
	return l.f(a) * l.f(b) <= 0;
}
point intersect(const line& a, const line& b) {
	return point(-(a.c * b.b - b.c * a.b) / (a.a * b.b - b.a * a.b), -(a.a * b.c - b.a * a.c) / (a.a * b.b - b.a * a.b));
}
point operator+(const point& a, const point& b) {
	return point(a.x + b.x, a.y + b.y);
}
point operator-(const point& a, const point& b) {
	return point(a.x - b.x, a.y - b.y);
}
point operator*(const point& a, ld x) {
	return point(a.x * x, a.y * x);
}
point operator/(const point& a, ld x) {
	return point(a.x / x, a.y / x);
}
ld operator*(const point& a, const point& b) {
	return a.x * b.x + a.y * b.y;
}
ld operator%(const point& a, const point& b) {
	return a.x * b.y - a.y * b.x;
}
point operator-(const point& a) {
	return point(-a.x, -a.y);
}
ld area(point a, point b, point c) {
	return point(b - a) % point(c - a);
}
bool intersect_(ld a, ld b, ld c, ld d) {
	if (a > b)std::swap(a, b);
	if (c > d)std::swap(c, d);
	return std::min(b, d) - std::max(a, c) >= -eps;
}
bool intersect(point a, point b, point c, point d) {
	return intersect_(a.x, b.x, c.x, d.x) && intersect_(a.y, b.y, c.y, d.y) && area(a, b, c) * area(a, b, d) <= eps && area(c, d, a) * area(c, d, b) <= eps;
}
bool intersect(point a, point b, const std::vector<point>& v) {
	for (int i = 0; i < v.size(); i++) {
		if (intersect(a, b, v[i], v[(i + 1) % (int)v.size()]))return true;
	}
	return false;
}
bool intersect(const std::vector<point>& a, const std::vector<point>& b) {
	for (int i = 0; i < a.size(); i++) {
		if (intersect(a[i], a[(i + 1) % (int)a.size()], b))return true;
	}
	return false;
}
ld dist2(point a, point b) {
	return (a - b).len2();
}
ld dist(point a, point b) {
	return (a - b).len();
}
ld dist(point a, point b, point x) {
	ld A = a.y - b.y, B = b.x - a.x, C = a.x * b.y - a.y * b.x;
	point v = b - a, v1 = x - a, v2 = x - b;
	if (v * v1 >= -eps && (-v) * v2 >= -eps) {
		return abs(A * x.x + B * x.y + C) / sqrt(A * A + B * B);
	}
	else {
		return std::min(dist(a, x), dist(b, x));
	}
}
bool isIn(const std::vector<point>& v, point x) {
	ld sq1 = 0;
	for (int i = 0; i < v.size(); i++) {
		point a = v[i] - v[0], b = v[(i + 1) % v.size()] - v[0];
		sq1 += abs(a % b);
	}
	ld sq2 = 0;
	for (int i = 0; i < v.size(); i++) {
		point a = v[i] - x, b = v[(i + 1) % v.size()] - x;
		sq2 += abs(a % b);
	}
	return (abs(sq1 - sq2) <= eps);
}
ld dist(const std::vector<point>& v, point x) {
	ld res = 1e9;
	for (int i = 0; i < v.size(); i++) {
		res = std::min(res, dist(v[i], v[(i + 1) % (int)v.size()], x));
	}
	if (isIn(v, x))res = -res;
	return res;
}
template<class T>
float getWidth(const T& t) {
	auto temp = t.getGlobalBounds();
	return temp.width;
}
template<class T>
float getHeight(const T& t) {
	auto temp = t.getGlobalBounds();
	return temp.height;
}
bool isNum(String s) {
	if (!s.isEmpty() && s[0] == '-')s.erase(0);
	if (s.isEmpty() || s[0] == '0' && s.getSize() != 1)return false;
	for (auto i : s) {
		if (!('0' <= i && i <= '9'))return false;
	}
	return true;
}
bool is16Num(String s) {
	if (!s.isEmpty() && s[0] == '-')s.erase(0);
	if (s.isEmpty())return false;
	for (auto i : s) {
		if (!('0' <= i && i <= '9') && !(('A' <= i && i <= 'F') || ('a' <= i && i <= 'f')))return false;
	}
	return true;
}
template<class T>
int width(const T& x) {
	FloatRect rect = x.getLocalBounds();
	return rect.width;
}
template<class T>
int height(const T& x) {
	FloatRect rect = x.getLocalBounds();
	return rect.height;
}
int GAMEMODE;
ld MOVEMENT_SPEED;
ld ROTATION_SPEED;
#define WALL_LENGTH 100
#define WALL_THICKNESS 4
#define TANK_LENGTH 38
#define TANK_WIDTH 28
#define BARREL_SIZE 7
#define BULLET_SIZE 3
ld BULLET_SPEED;
ld BULLET_LIFETIME;
int BULLET_BOUNCES;
ld RELOAD_TIME;
#define UPDATE_TIME (ld)0.003
#define BORDER_SIZE 10
#define MAX_BULLET_COUNT 5
#define POWERUP_SIZE 30
#define LASER_DISTANCE 1000
#define LASER_LIFETIME 0.5
#define SHARD_SPEED 800
#define WALL_DESTROYER_DISTANCE 100
#define WALL_DESTROYER_RADIUS 10
std::string gamemodes_names[3] = { "Normal mode", "Sniper mode", "Laser mode" };
int WIDTH, HEIGHT;
int sx, sy;
ld scale;
std::mutex mtxD;
ld getdrawx(ld x) {
	return x * scale + sx;
}
ld getdrawy(ld y) {
	return y * scale + sy;
}
void drawLineSegment(RenderWindow& window, ld x1, ld y1, ld x2, ld y2, const Color& color, int width = 1) {
	x1 = getdrawx(x1);
	y1 = getdrawy(y1);
	x2 = getdrawx(x2);
	y2 = getdrawy(y2);
	VertexArray quad(Quads, 4);
	ld dx = x2 - x1, dy = y2 - y1;
	ld tx = dx, ty = dy;
	ld len = point(tx, ty).len();
	tx /= len;
	ty /= len;
	std::swap(tx, ty);
	tx *= -1;
	tx /= 2;
	ty /= 2;
	tx *= width;
	ty *= width;
	quad[0].position = Vector2f(x1 - tx, y1 - ty);
	quad[1].position = Vector2f(x1 + tx, y1 + ty);
	quad[2].position = Vector2f(x2 + tx, y2 + ty);
	quad[3].position = Vector2f(x2 - tx, y2 - ty);
	for (int i = 0; i < 4; i++)quad[i].color = color;
	window.draw(quad);
}
struct wall {
	int x, y;
	bool vertical;
	RectangleShape rect;
	std::vector<point>v;
	wall(int _x, int _y, bool _vertical) :x(_x), y(_y), vertical(_vertical) {
		rect.setRotation(vertical * 90);
		rect.setFillColor(Color(77, 77, 77, 255));
		rect.setOutlineColor(Color(77, 77, 77, 255));
		rect.setOrigin(Vector2f(WALL_LENGTH / 2, WALL_THICKNESS / 2));
		rect.setSize(Vector2f(WALL_LENGTH, WALL_THICKNESS));
		if (!vertical) {
			v.push_back(point(x - WALL_LENGTH / 2, y - WALL_THICKNESS / 2));
			v.push_back(point(x + WALL_LENGTH / 2, y - WALL_THICKNESS / 2));
			v.push_back(point(x + WALL_LENGTH / 2, y + WALL_THICKNESS / 2));
			v.push_back(point(x - WALL_LENGTH / 2, y + WALL_THICKNESS / 2));
		}
		else {
			v.push_back(point(x - WALL_THICKNESS / 2, y - WALL_LENGTH / 2));
			v.push_back(point(x + WALL_THICKNESS / 2, y - WALL_LENGTH / 2));
			v.push_back(point(x + WALL_THICKNESS / 2, y + WALL_LENGTH / 2));
			v.push_back(point(x - WALL_THICKNESS / 2, y + WALL_LENGTH / 2));
		}
	}
	void draw(RenderWindow& window) {
		rect.setPosition(getdrawx(x), getdrawy(y));
		rect.setScale(scale, scale);
		window.draw(rect);
	}
	bool operator==(const wall& w)const {
		return x == w.x && y == w.y && vertical == w.vertical;
	}
};
std::mt19937 mapBuilder;
long long Mrandnum(long long l, long long r) {
	return l + mapBuilder() % (r - l + 1);
}
struct walls {
	std::vector<wall>w;
	std::vector<std::pair<ld, std::vector<bool>>>wx, wy;
	int mxx, mxy;
	std::mutex mtx;
	walls() :mxx(0), mxy(0) {}
	void insert(const wall& c) {
		mtx.lock();
		w.push_back(c);
		mtx.unlock();
	}
	int size()const {
		return w.size();
	}
	void clear() {
		mtx.lock();
		w.clear();
		wx.clear();
		wy.clear();
		mxx = 0;
		mxy = 0;
		mtx.unlock();
	}
	void eraseRandom(int cnt) {
		mtx.lock();
		for (int i = 0; i < cnt; i++) {
			w.erase(w.begin() + Mrandnum(0, (int)w.size() - 1));
		}
		mtx.unlock();
	}
	void build() {
		mtx.lock();
		wx.clear();
		wy.clear();
		mxx = 0;
		mxy = 0;
		for (auto i : w) {
			for (auto j : i.v) {
				mxx = std::max(mxx, (int)j.x);
				mxy = std::max(mxy, (int)j.y);
			}
		}
		std::map<ld, std::vector<bool>>mx, my;
		for (auto i : w) {
			if (mx.find(i.v[0].x) == mx.end()) {
				mx[i.v[0].x] = std::vector<bool>(mxy, false);
			}
			if (mx.find(i.v[1].x) == mx.end()) {
				mx[i.v[1].x] = std::vector<bool>(mxy, false);
			}
			for (int j = i.v[1].y; j < i.v[2].y; j++) {
				mx[i.v[0].x][j] = true;
				mx[i.v[1].x][j] = true;
			}
			if (my.find(i.v[1].y) == my.end()) {
				my[i.v[1].y] = std::vector<bool>(mxx, false);
			}
			if (my.find(i.v[2].y) == my.end()) {
				my[i.v[2].y] = std::vector<bool>(mxx, false);
			}
			for (int j = i.v[0].x; j < i.v[1].x; j++) {
				my[i.v[1].y][j] = true;
				my[i.v[2].y][j] = true;
			}
		}
		for (auto i : mx)wx.push_back(i);
		for (auto i : my)wy.push_back(i);
		mtx.unlock();
	}
	std::pair<point, bool>findIntersection(point x, point v)const {
		line cur(x, v);
		point res = x + v * 1e9;
		ld mn = 1e9;
		bool vertical = false;
		for (auto i : wx) {
			ld cury = cur.gety(i.first);
			if ((int)cury < 0 || (int)cury >= mxy || !i.second[(int)cury])continue;
			point p(i.first, cury);
			if (v * (p - x) < 1e-5)continue;
			ld d = dist(x, p);
			if (d < mn) {
				mn = d;
				vertical = true;
				res = p;
			}
		}
		for (auto i : wy) {
			ld curx = cur.getx(i.first);
			if ((int)curx < 0 || (int)curx >= mxx || !i.second[(int)curx])continue;
			point p(curx, i.first);
			if (v * (p - x) < 1e-5)continue;
			ld d = dist(x, p);
			if (d < mn) {
				mn = d;
				vertical = false;
				res = p;
			}
		}
		return std::make_pair(res, vertical);
	}
	std::vector<std::pair<ld, point>>getTrajectory(ld x, ld y, ld angle, ld speed, ld lifetime, int bounces) {
		mtx.lock();
		point s = point(x, y);
		point v = point(cos(angle), sin(angle));
		std::vector<std::pair<ld, point>>path;
		path.push_back({ 0, s });
		while (path.back().first < lifetime && (bounces == -1 || path.size() < bounces + 2)) {
			auto temp = findIntersection(path.back().second, v);
			path.push_back({ path.back().first + dist(path.back().second, temp.first) / speed, temp.first });
			if (!temp.second)v.y = -v.y;
			else v.x = -v.x;
		}
		if (path.back().first > lifetime) {
			point v = path.back().second - path[(int)path.size() - 2].second;
			v = v / v.len();
			v = v * ((lifetime - path[(int)path.size() - 2].first) * speed);
			path.back().second = path[(int)path.size() - 2].second + v;
			path.back().first = lifetime;
		}
		mtx.unlock();
		return path;
	}
	void draw(RenderWindow& window) {
		mtx.lock();
		for (auto i : w)i.draw(window);
		mtx.unlock();
	}
};
struct bullet {
	ld x, y, angle;
	ld startx, starty, startAngle;
	ld timer;
	int id, uid;
	bool everBounced;
	std::vector<std::pair<ld, point>>path;
	int pos;
	Uint8 type;
	ld timeCreated;
	ld speed;
	bullet(ld _x, ld _y, ld _angle, int _id, int _uid, Uint8 _type, walls& w, ld _timeCreated, ld minusTime = 0) :x(_x), y(_y), timer(0), id(_id), type(_type), everBounced(false), pos(0) {
		startx = _x;
		starty = _y;
		startAngle = _angle;
		uid = _uid;
		timeCreated = _timeCreated;
		angle = startAngle;
		ld curBounces;
		if (type == 0) {
			speed = BULLET_SPEED;
			curBounces = BULLET_BOUNCES;
		}
		if (type == 1) {
			speed = BULLET_SPEED;
			curBounces = BULLET_BOUNCES;
		}
		if (type == 2) {
			speed = SHARD_SPEED;
			curBounces = 0;
		}
		if (type == 3) {
			speed = 1600;
			curBounces = 3;
		}
		path = w.getTrajectory(x, y, angle, speed, BULLET_LIFETIME - minusTime, curBounces);
	}
	void setPosition() {
		if (pos < path.size()) {
			point v = path[pos].second - path[pos - 1].second;
			v = v / v.len() * ((timer - path[pos - 1].first) * speed);
			x = path[pos - 1].second.x + v.x;
			y = path[pos - 1].second.y + v.y;
			angle = std::atan2(path[pos].second.y - path[pos - 1].second.y, path[pos].second.x - path[pos - 1].second.x);
		}
		else {
			x = path.back().second.x;
			y = path.back().second.y;
		}
	}
	void setTime(ld t) {
		timer = t - timeCreated;
		while (pos < path.size() && path[pos].first < timer)pos++;
		while (pos > 0 && path[pos - 1].first > timer)pos--;
		if (timer >= BULLET_LIFETIME)return;
		setPosition();
	}
	void move(ld dt) {
		setTime(timer += dt);
	}
	void draw(RenderWindow& window) {
		int curBulletSize = BULLET_SIZE;
		if (type == 1)curBulletSize++;
		if (type == 2)curBulletSize--;
		CircleShape c(scale * curBulletSize);
		c.setPosition(Vector2f(sx + scale * x - scale * BULLET_SIZE, sy + scale * y - scale * BULLET_SIZE));
		c.setFillColor(Color(0, 0, 0, 255));
		c.setOutlineColor(Color(0, 0, 0, 255));
		window.draw(c);
		/*for (int i = 0; i < (int)path.size() - 1; i++) {
			drawLineSegment(window, path[i].second.x, path[i].second.y, path[i + 1].second.x, path[i + 1].second.y, Color::Red);
		}*/
	}
};
Sprite BonusSprite[7];
struct powerup {
	ld x, y;
	Uint8 type;
	int uid;
	std::vector<point>box;
	powerup(ld _x, ld _y, Uint8 _type, int _uid) : x(_x), y(_y), type(_type), uid(_uid) {
		box.push_back(point(x - POWERUP_SIZE / 2, y - POWERUP_SIZE / 2));
		box.push_back(point(x - POWERUP_SIZE / 2, y + POWERUP_SIZE / 2));
		box.push_back(point(x + POWERUP_SIZE / 2, y - POWERUP_SIZE / 2));
		box.push_back(point(x + POWERUP_SIZE / 2, y + POWERUP_SIZE / 2));
	}
	void draw(RenderWindow& window) {
		Sprite sprite;
		if (type < sizeof(BonusSprite) / sizeof(Sprite))sprite = BonusSprite[type];
		sprite.setScale(scale * (ld)POWERUP_SIZE / sprite.getTextureRect().width, scale * (ld)POWERUP_SIZE / sprite.getTextureRect().height);
		sprite.setPosition(getdrawx(x - POWERUP_SIZE / 2), getdrawy(y - POWERUP_SIZE / 2));
		window.draw(sprite);
		if (type >= 7) {
			RectangleShape rect;
			rect.setFillColor(Color::Red);
			rect.setSize(Vector2f(scale * POWERUP_SIZE, scale * POWERUP_SIZE));
			rect.setPosition(getdrawx(x - POWERUP_SIZE / 2), getdrawy(y - POWERUP_SIZE / 2));
			window.draw(rect);
		}
	}
};
struct laser {
	ld x, y, angle;
	ld timer;
	int uid;
	std::vector<std::pair<ld, point>>path;
	void buildTrajectory(walls& w) {
		path = w.getTrajectory(x, y, angle, 1, LASER_DISTANCE, -1);
	}
	laser(ld _x, ld _y, ld _angle, int _uid, walls& w) :x(_x), y(_y), angle(_angle), timer(0), uid(_uid) {
		buildTrajectory(w);
	}
	void setTime(ld t) {
		timer = t;
	}
	void update(ld dt) {
		setTime(timer + dt);
	}
};
Sprite TankSprite[3];
struct tank {
	ld x, y, angle;
	int id;
	bool dead;
	int color;
	Uint8 bonus;
	ld lastShot;
	tank() : x(0), y(0), angle(0), dead(true) {}
	void draw(RenderWindow& window, bool isThisPlayer) {
		if (dead)return;
		Sprite sprite;
		if (bonus == 4) {
			sprite = TankSprite[1];
		}
		else if (bonus == 5 || GAMEMODE == 1) {
			sprite = TankSprite[2];
		}
		else {
			sprite = TankSprite[0];
		}
		sprite.setOrigin(TANK_LENGTH / 2, TANK_WIDTH / 2);
		sprite.setScale(scale * (TANK_LENGTH + BARREL_SIZE) / sprite.getTextureRect().width, scale * TANK_WIDTH / sprite.getTextureRect().height);
		sprite.setPosition(sx + x * scale, sy + y * scale);
		sprite.setRotation(angle / M_PI * (ld)180);
		Color temp(color);
		if (isThisPlayer && (bonus == 2))temp.a = 63;
		sprite.setColor(temp);
		window.draw(sprite);
	}
	std::vector<point>getRect() {
		std::vector<point>a;
		for (int i = 0; i < 4; i++) {
			ld dx = (TANK_LENGTH / 2 - 1) * cosl(angle), dy = (TANK_LENGTH / 2 - 1) * sinl(angle);
			ld dx_ = dx, dy_ = dy;
			std::swap(dx_, dy_);
			dx_ *= -1;
			dx_ /= len(dx, dy);
			dy_ /= len(dx, dy);
			dx_ *= TANK_WIDTH / 2 - 1;
			dy_ *= TANK_WIDTH / 2 - 1;
			if (i & 1) {
				dx *= -1;
				dy *= -1;
			}
			if (i & 2) {
				dx_ *= -1;
				dy_ *= -1;
			}
			dx += dx_;
			dy += dy_;
			a.push_back(point(x + dx, y + dy));
		}
		std::swap(a[2], a[3]);
		return a;
	}
	bool check(const walls& w) {
		std::vector<point>a = getRect();
		int len2 = sqrt(TANK_LENGTH * TANK_LENGTH / 4 + TANK_WIDTH * TANK_WIDTH / 4) + sqrt(WALL_THICKNESS * WALL_THICKNESS / 4 + WALL_LENGTH * WALL_LENGTH / 4) + 5;
		len2 *= len2;
		for (auto& i : w.w) {
			if (dist2(point(x, y), point(i.x, i.y)) > len2)continue;
			std::vector<point>b = i.v;
			if (intersect(a, b)) {
				return false;
			}
		}
		return true;
	}
	void move(const walls& w, ld t) {
		if (dead)return;
		ld oldx = x, oldy = y;
		x += MOVEMENT_SPEED * t * cosl(angle);
		y += MOVEMENT_SPEED * t * sinl(angle);
		if (!check(w)) {
			x = oldx;
			y = oldy;
			ld oldangle = angle;
			for (int i = 0; i < 10; i++) {
				angle = oldangle + (ld)randnum(-1000, 1000) / (ld)50000;
				if (check(w))break;
				if (i == 9) {
					angle = oldangle;
				}
			}
		}
	}
	void rotate(const walls& w, ld t) {
		if (dead)return;
		ld oldangle = angle;
		angle += ((ld)ROTATION_SPEED / (ld)180 * M_PI) * t;
		if (!check(w)) {
			angle = oldangle;
			ld oldx = x, oldy = y;
			for (int i = 0; i < 10; i++) {
				x = oldx + randnum(-2, 2);
				y = oldy + randnum(-2, 2);
				if (check(w))break;
				if (i == 9) {
					x = oldx;
					y = oldy;
				}
			}
		}
	}
	void remove() {
		dead = true;
	}
};
struct player {
	tank t;
	int pressed;
	std::string nickname;
	bool connected, everConnected;
	int points;
	Text nicknameText, reloadText;
	player() :pressed(0), nickname(""), connected(false), everConnected(false), points(0), t(tank()) {}
	void changeNickname(std::string _nickname) {
		nickname = _nickname;
		nicknameText.setCharacterSize(15);
		nicknameText.setString(nickname);
		nicknameText.setFillColor(Color(127, 127, 127, 191));
		nicknameText.setOrigin(getWidth(nicknameText) / 2, getHeight(nicknameText));
		reloadText.setCharacterSize(25);
	}
	void connect() {
		connected = true;
	}
	void disconnect() {
		connected = false;
	}
	void checkAction(const walls& w, ld dt) {
		if (pressed & 1) {
			t.move(w, dt);
		}
		if (pressed & 2) {
			t.move(w, -dt / (ld)2);
		}
		if (pressed & 4) {
			t.rotate(w, -dt);
		}
		if (pressed & 8) {
			t.rotate(w, dt);
		}
	}
	void draw(RenderWindow& window, bool isThisPlayer, ld timeSinceStart) {
		if (!t.dead) {
			if (!isThisPlayer && (t.bonus == 2))return;
			t.draw(window, isThisPlayer);
			if (!isThisPlayer) {
				nicknameText.setPosition(getdrawx(t.x), getdrawy(t.y) - TANK_WIDTH / 2 - 18 * scale);
				window.draw(nicknameText);
			}
			else {
				if (timeSinceStart - t.lastShot <= RELOAD_TIME) {
					ld untilReload = RELOAD_TIME - (timeSinceStart - t.lastShot);
					reloadText.setPosition(getdrawx(t.x), getdrawy(t.y) - TANK_WIDTH / 2 - 18 * scale);
					std::string reloadString;
					int temp = untilReload * 10;
					reloadString = std::to_string(temp);
					if (reloadString.size() == 1)reloadString.insert(reloadString.begin(), '0');
					reloadString.insert(reloadString.begin() + 1, '.');
					reloadText.setString(reloadString);
					reloadText.setOrigin(getWidth(reloadText) / 2, getHeight(reloadText));
					reloadText.setFillColor(Color(255, 0, 0, 255 * (untilReload / RELOAD_TIME)));
					window.draw(reloadText);
				}
			}
		}
	}
};
struct Chat {
	std::vector<std::pair<ld, String>>messages;
	std::vector<String>sent;
	String curMessage, oldMessage;
	bool typingNickname;
	String curAutoPart;
	String curAuto;
	bool typing;
	int selectedMessagePos;
	int posStartDrawing;
	int messagePos;
	ld lastCursorMoved;

	std::mutex mtx;
	Chat(): typing(false), typingNickname(false) {}
	bool wordPart(Uint32 c) {
		return '0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_' || 1040 <= c && c <= 1103 || c == 1105 || c == 1025;
	}
	Uint32 lower(Uint32 c) {
		if ('A' <= c && c <= 'Z') {
			return c - 'A' + 'a';
		}
		return c;
	}
	String getCurWord() {
		String res;
		for (int i = messagePos - 1; i >= -1; i--) {
			if (i == -1 || !wordPart(curMessage[i]) && curMessage[i] != '/')break;
			res += curMessage[i];
		}
		std::reverse(res.begin(), res.end());
		return res;
	}
	void putChar(Uint32 c, std::vector<String>nicknames = std::vector<String>()) {
		mtx.lock();
		lastCursorMoved = gettime();
		if (c == 13) {}
		else if (c == 8) {
			typingNickname = false;
			if (messagePos > 0) {
				curMessage.erase(messagePos-- - 1);
			}
		}
		else if (c == 127) {
			typingNickname = false;
			bool everWordPart = false;
			for (int i = messagePos - 1; i >= -1; i--) {
				if (i == -1 || everWordPart && !wordPart(curMessage[i])) {
					curMessage.erase(i + 1, messagePos - (i + 1) + 1);
					messagePos = i + 1;
					break;
				}
				if (wordPart(curMessage[i]))everWordPart = true;
			}
		}
		else if (c == 9) {
			if (!typingNickname) {
				typingNickname = true;
				curAutoPart.clear();
				curAuto.clear();
				curAutoPart = getCurWord();
			}
			std::sort(nicknames.begin(), nicknames.end());
			std::vector<String>prefixOf;
			for (auto s : nicknames) {
				if (curAutoPart.getSize() > s.getSize())continue;
				bool isPrefix = true;
				for (int i = 0; i < curAutoPart.getSize(); i++) {
					if (lower(s[i]) != lower(curAutoPart[i])) {
						isPrefix = false;
						break;
					}
				}
				if (isPrefix && !(curAutoPart.getSize() == 0 && !s.isEmpty() && s[0] == '/')) {
					prefixOf.push_back(s);
				}
			}
			if (!prefixOf.empty()) {
				bool found = false;
				for (int i = 0; i < prefixOf.size(); i++) {
					if (curAuto == prefixOf[i]) {
						curAuto = prefixOf[(i + 1) % (int)prefixOf.size()];
						found = true;
						break;
					}
				}
				if (!found)curAuto = prefixOf[0];
				for (int i = messagePos - 1; i >= -1; i--) {
					if (i == -1 || !wordPart(curMessage[i]) && curMessage[i] != '/') {
						curMessage.erase(i + 1, messagePos - i - 1);
						curMessage.insert(i + 1, curAuto);
						messagePos = i + 1 + curAuto.getSize();
						break;
					}
				}
			}
		}
		else {
			typingNickname = false;
			if (curMessage.getSize() < 150) {
				curMessage.insert(messagePos, c);
				messagePos++;
			}
		}
		mtx.unlock();
	}
	void deleteSymbol() {
		mtx.lock();
		typingNickname = false;
		if (messagePos < curMessage.getSize()) {
			curMessage.erase(messagePos);
			lastCursorMoved = gettime();
		}
		mtx.unlock();
	}
	void putMessage(String message) {
		mtx.lock();
		messages.push_back(std::make_pair(gettime(), message));
		if (posStartDrawing == (int)messages.size() - 21)posStartDrawing++;
		mtx.unlock();
	}
	void send() {
		mtx.lock();
		sent.push_back(curMessage);
		mtx.unlock();
	}
	void setTyping(bool flag) {
		mtx.lock();
		typing = flag;
		if (!typing) {
			curMessage.clear();
		}
		else {
			typingNickname = false;
			selectedMessagePos = sent.size();
			posStartDrawing = std::max(0, (int)messages.size() - 20);
			messagePos = 0;
			lastCursorMoved = gettime();
		}
		mtx.unlock();
	}
	bool getTyping() {
		return typing;
	}
	String getMessage() {
		mtx.lock();
		String returnMessage = curMessage;
		mtx.unlock();
		return returnMessage;
	}
	void goUp() {
		mtx.lock();
		typingNickname = false;
		int oldPos = selectedMessagePos;
		if (selectedMessagePos == sent.size())oldMessage = curMessage;
		selectedMessagePos = std::max(0, selectedMessagePos - 1);
		if (selectedMessagePos != sent.size())curMessage = sent[selectedMessagePos];
		else curMessage = oldMessage;
		if (oldPos != selectedMessagePos)messagePos = curMessage.getSize();
		lastCursorMoved = gettime();
		mtx.unlock();
	}
	void goDown() {
		mtx.lock();
		typingNickname = false;
		int oldPos = selectedMessagePos;
		selectedMessagePos = std::min((int)sent.size(), selectedMessagePos + 1);
		if (selectedMessagePos != sent.size())curMessage = sent[selectedMessagePos];
		else curMessage = oldMessage;
		if (oldPos != selectedMessagePos)messagePos = curMessage.getSize();
		lastCursorMoved = gettime();
		mtx.unlock();
	}
	void scroll(int steps) {
		mtx.lock();
		posStartDrawing = std::min(std::max(0, posStartDrawing - steps), std::max(0, (int)messages.size() - 20));
		mtx.unlock();
	}
	void moveCursor(int steps) {
		typingNickname = false;
		messagePos = std::max(0, std::min((int)curMessage.getSize(), messagePos + steps));
		lastCursorMoved = gettime();
	}
	void moveCursorCtrl(int direction) {
		mtx.lock();
		typingNickname = false;
		while (true) {
			moveCursor(direction);
			if (messagePos == 0 || messagePos == curMessage.getSize())break;
			if (!wordPart(curMessage[messagePos - 1]) && wordPart(curMessage[messagePos]))break;
		}
		mtx.unlock();
	}
	void draw(RenderWindow &window, const Font& font) {
		mtx.lock();
		int start = (!typing ? (std::max(0, (int)messages.size() - 3)) : posStartDrawing);
		int cnt = (!typing ? cnt = std::min((int)messages.size(), 3) : std::min((int)messages.size() - posStartDrawing, 20));
		for (int i = start; i < start + cnt; i++) {
			int alpha = 191;
			ld dt = gettime() - messages[i].first;
			if (dt >= 6 && !typing) {
				alpha -= 191 * (dt - 6);
			}
			if (alpha <= 0)continue;
			Text temp(messages[i].second, font, 20);
			temp.setPosition(20, HEIGHT - 1 - 30 - (start + cnt - i) * 30);
			temp.setFillColor(Color(63, 63, 63, alpha));
			window.draw(temp);
		}
		if (typing) {
			Text temp(curMessage, font, 20);
			temp.setPosition(20, HEIGHT - 1 - 30);
			temp.setFillColor(Color(63, 63, 63, 191));
			if ((int)((gettime() - lastCursorMoved) * 10) % 10 < 5) {
				Text part(curMessage.substring(0, messagePos), font, 20);
				part.setPosition(20, HEIGHT - 1 - 30);
				auto frect = part.getGlobalBounds();
				RectangleShape tempRect;
				tempRect.setPosition(frect.left + frect.width, HEIGHT - 1 - 26);
				tempRect.setSize(Vector2f(3, 20));
				tempRect.setFillColor(Color(63, 63, 63, 191));
				window.draw(tempRect);
			}
			window.draw(temp);
		}
		mtx.unlock();
	}
};
void dfs(int x, int y, std::vector<std::vector<bool>>& used, std::vector<std::vector<bool>>& hwall, std::vector<std::vector<bool>>& vwall) {
	used[x][y] = true;
	std::vector<int>cur;
	for (int i = 0; i < 4; i++)cur.push_back(i);
	std::shuffle(cur.begin(), cur.end(), mapBuilder);
	for (auto z : cur) {
		if (z == 0 && x + 1 < used.size()) {
			if (!used[x + 1][y]) {
				vwall[x][y] = false;
				dfs(x + 1, y, used, hwall, vwall);
			}
		}
		if (z == 1 && y + 1 < used[x].size()) {
			if (!used[x][y + 1]) {
				hwall[x][y] = false;
				dfs(x, y + 1, used, hwall, vwall);
			}
		}
		if (z == 2 && x - 1 >= 0) {
			if (!used[x - 1][y]) {
				vwall[x - 1][y] = false;
				dfs(x - 1, y, used, hwall, vwall);
			}
		}
		if (z == 3 && y - 1 >= 0) {
			if (!used[x][y - 1]) {
				hwall[x][y - 1] = false;
				dfs(x, y - 1, used, hwall, vwall);
			}
		}
	}
}
wall geth(int i, int j) {
	int x = ((i == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * i));
	int y = ((j == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * j));
	return wall(x + WALL_LENGTH / 2, y + WALL_THICKNESS / 2, false);
}
wall getv(int i, int j) {
	int x = ((i == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * i));
	int y = ((j == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * j));
	return wall(x + WALL_THICKNESS / 2, y + WALL_LENGTH / 2, true);
}
ld getMapX(int i) {
	return ((WALL_LENGTH - WALL_THICKNESS) * i) + WALL_LENGTH / 2;
}
ld getMapY(int j) {
	return ((WALL_LENGTH - WALL_THICKNESS) * j) + WALL_LENGTH / 2;
}
void positionTank(tank& t, int i, int j, ld angle) {
	t.x = getMapX(i);
	t.y = getMapY(j);
	t.angle = angle;
}
std::vector<player>t;
std::mutex wMtx;
walls w;
std::mutex bMtx;
std::vector<bullet>b;
std::mutex pMtx;
std::vector<powerup>p;
std::mutex lasersMtx;
std::vector<laser>lasers;
bool windowOpen;
int mapLength, mapHeight;
int mapSeed = -1;
int tankNum = -1;
std::mutex pingMtx;
std::deque<std::pair<ld, std::pair<int, bool>>>pingPackets;
void rescale() {
	int length = (WALL_LENGTH - WALL_THICKNESS) * mapLength + WALL_THICKNESS;
	int height = (WALL_LENGTH - WALL_THICKNESS) * mapHeight + WALL_THICKNESS;
	scale = 1;
	scale = std::min(scale, (ld)(WIDTH - 2 * BORDER_SIZE) / length);
	scale = std::min(scale, (ld)(HEIGHT - 2 * BORDER_SIZE - 50) / height);
	int realLength = length * scale;
	int realHeight = height * scale;
	sx = (WIDTH - realLength) / 2;
	sy = (HEIGHT - realHeight - 50) / 2;
}
void buildMap() {
	mtxD.lock();
	w.build();
	rescale();
	mtxD.unlock();
}
template<class T>
void centerDownOrigin(T& t) {
	t.setOrigin((getLength(t) - 1) / 2, getHeight(t) - 1);
}
bool inFocus;
Chat chat;
std::string Nickname, Password, sPassword;
TcpSocket sock;
IpAddress ip = "217.107.197.134";
unsigned short port = 13111;
#define BUFFER_SIZE 256
int PLAYER_COUNT = 0;
std::deque<Packet>rc[256];
std::mutex rcMtx;
bool handling = true;
bool connected = false;
ld timeSinceLastPacket;
std::set<std::pair<int, ld>>packetsUidUsed1;
std::queue<std::pair<ld, int>>packetsUidUsed2;
std::mutex packetsUidReceivedMtx;
std::set<std::pair<int, ld>>packetsUidReceived1;
std::set<std::pair<ld, int>>packetsUidReceived2;
void handlingPackets() {
	while (handling) {
		Packet rPacket;
		if (sock.receive(rPacket) == Socket::Done) {
			Uint8 header;
			rPacket >> header;
			rcMtx.lock();
			rc[header].push_back(rPacket);
			if (rc[header].size() > PACKET_STORAGE_SIZE) {
				rc[header].pop_back();
			}
			rcMtx.unlock();
		}
	}
}
std::pair<int, String>connect(std::string nickname, std::string password, std::string spassword) {
	if (nickname.size() > 15 || nickname.size() < 1)return { 1, "" };
	for (auto i : nickname) {
		if (!(33 <= i && i <= 126))return{ 1, "" };
	}
	Packet toSend;
	toSend << (Uint8)'C';
	toSend << nickname;
	toSend << password;
	toSend << spassword;
	ld starttime = gettime();
	sock.disconnect();
	sock.connect(ip, port, seconds(DISCONNECT_AFTER));
	sock.send(toSend);
	while (gettime() - starttime <= DISCONNECT_AFTER) {
		Sleep(1);
		if (!windowOpen) {
			break;
		}
		rcMtx.lock();
		if (!rc['C'].empty()) {
			rc['C'].pop_front();
			rcMtx.unlock();
			return { 0, "" };
		}
		else if (!rc['R'].empty()) {
			String answer;
			rc['R'].front() >> answer;
			rc['R'].pop_front();
			rcMtx.unlock();
			return { 2, answer };
		}
		else rcMtx.unlock();
	}
	return { 1, "" };
}
std::pair<String, String>parseString(const String& s) {
	for (int i = 0; i < s.getSize(); i++) {
		if (s[i] == '=') {
			String a = s.substring(0, i), b = s.substring(i + 1, (int)s.getSize() - i - 1);
			while (!a.isEmpty() && a[(int)a.getSize() - 1] == ' ')a.erase((int)a.getSize() - 1);
			std::reverse(b.begin(), b.end());
			while (!b.isEmpty() && b[(int)b.getSize() - 1] == ' ')b.erase((int)b.getSize() - 1);
			std::reverse(b.begin(), b.end());
			return std::make_pair(a, b);
		}
	}
	return std::make_pair(s, "");
}
std::vector<std::pair<std::string, std::string>>parseInput(std::ifstream& in) {
	std::vector<std::pair<std::string, std::string>>res;
	std::string cur;
	while (std::getline(in, cur)) {
		auto temp = parseString(cur);
		res.push_back(temp);
	}
	return res;
}
std::vector<String>parseCommand(String command) {
	command += ' ';
	std::vector<String>res;
	int pos = 0;
	for (int i = 0; i < command.getSize(); i++) {
		if (command[i] == ' ') {
			if (i != pos)res.push_back(command.substring(pos, i - pos));
			pos = i + 1;
		}
	}
	return res;
}
std::pair<String, unsigned short>parseIpPort(String s) {
	int found1 = -1;
	for (int i = 0; i < s.getSize(); i++) {
		if (s[i] == ':') {
			found1 = i;
			break;
		}
	}
	unsigned short resPort = 13111;
	if (found1 != -1) {
		String temp = s.substring(found1 + 1, (int)s.getSize() - 1 - found1);
		if (isNum(temp) && temp.getSize() <= 5) {
			int num = std::stoi((std::string)temp);
			if (0 <= num && num <= (1 << 16) - 1)resPort = num;
		}
		else return std::make_pair("", 0);
	}
	if (found1 != -1)s = s.substring(0, found1);
	s += '.';
	int l = 0;
	int cnt = 0;
	for (int i = 0; i < s.getSize(); i++) {
		if (s[i] == '.') {
			String temp = s.substring(l, i - l);
			if (isNum(temp) && temp.getSize() <= 3) {
				int num = std::stoi((std::string)temp);
				if (!(0 <= num && num <= 255))return std::make_pair("", 0);
			}
			l = i + 1;
			cnt++;
		}
	}
	if (cnt != 4)return std::make_pair("", 0);
	return std::make_pair(s, resPort);
}
int windowLength, windowHeight;
int windowX, windowY;
bool fullscreen;
bool useKeyboard;
void saveToSettings() {
	std::ofstream out("settings.txt");
	out << "nickname=" << Nickname << '\n';
	out << "password=" << Password << '\n';
	out << "server_password=" << sPassword << '\n';
	out << "window_length=" << windowLength << '\n';
	out << "window_height=" << windowHeight << '\n';
	out << "window_Xpos=" << windowX << '\n';
	out << "window_Ypos=" << windowY << '\n';
	out << "fullscreen_mode=" << (int)fullscreen << '\n';
	out << "use_keyboard=" << (int)useKeyboard << '\n';
	out.close();
}
void refreshWindow(RenderWindow& window) {
	window.close();
	if (!fullscreen) {
		WIDTH = windowLength;
		HEIGHT = windowHeight;
		window.create(VideoMode(WIDTH, HEIGHT), "Game", Style::Default);
		window.setPosition(Vector2i(windowX, windowY));
	}
	else {
		windowLength = WIDTH;
		windowHeight = HEIGHT;
		WIDTH = VideoMode::getFullscreenModes()[0].width;
		HEIGHT = VideoMode::getFullscreenModes()[0].height;
		window.create(VideoMode(WIDTH, HEIGHT), "Game", Style::Fullscreen);
	}
	window.setVerticalSyncEnabled(true);
	if (tankNum != -1)rescale();
	saveToSettings();
}
bool closeGame = false;
bool started = false;
ld mousex, mousey;
std::mutex bcMtx;
std::vector<std::pair<String, std::pair<int, int>>>broadcastMessages;
std::mutex listMtx;
std::vector<std::string>ncList;
ld timeSinceStart;
void drawing() {
	if (!windowOpen)return;
	RenderWindow window;
	refreshWindow(window);
	inFocus = true;
	std::deque<ld>frameTimes;
	ld oldTime = gettime();
	Font font;
	font.loadFromFile("Resources\\font.ttf");
	Text fps, ping, points, broadcastText;
	fps.setCharacterSize(20);
	fps.setFont(font);
	fps.setFillColor(Color::Red);
	ping.setCharacterSize(15);
	ping.setFont(font);
	ping.setFillColor(Color::Black);
	points.setCharacterSize(20);
	points.setFont(font);
	points.setFillColor(Color(127, 127, 127, 191));
	points.setPosition(0, 0);
	broadcastText.setFont(font);
	broadcastText.setPosition(0, 0);
	bool f11pressed = false;
	int oldWindowX = windowX, oldWindowY = windowY;
	bool tPressed = false, slashPressed = false, enterPressed = false;
	bool tabPressed = false;
	while (window.isOpen()) {
		if (!windowOpen) {
			window.close();
		}
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::GainedFocus)
				inFocus = true;
			else if (event.type == sf::Event::LostFocus)
				inFocus = false;
			else if (event.type == Event::Resized) {
				windowLength = window.getSize().x;
				windowHeight = window.getSize().y;
				WIDTH = windowLength;
				HEIGHT = windowHeight;
				windowX = window.getPosition().x;
				windowY = window.getPosition().y;
				refreshWindow(window);
			}
			else if (event.type == Event::TextEntered) {
				Uint32 cur = event.text.unicode;
				if (chat.getTyping()) {
					listMtx.lock();
					std::vector<String>temp;
					for (auto i : ncList)temp.push_back(i);
					listMtx.unlock();
					if (cur == 9 && !chat.typingNickname) {
						chat.mtx.lock();
						String curWord = chat.getCurWord();
						chat.mtx.unlock();
						String toPrint;
						std::sort(temp.begin(), temp.end());
						for (auto s : temp) {
							if (curWord.getSize() > s.getSize())continue;
							bool isPrefix = true;
							for (int i = 0; i < curWord.getSize(); i++) {
								if (chat.lower(s[i]) != chat.lower(curWord[i])) {
									isPrefix = false;
									break;
								}
							}
							if (isPrefix && !(curWord.getSize() == 0 && !s.isEmpty() && s[0] == '/')) {
								toPrint += s + ", ";
							}
						}
						if (!toPrint.isEmpty()) {
							chat.putMessage(toPrint.substring(0, (int)toPrint.getSize() - 2));
						}
					}
					chat.putChar(cur, temp);
				}
			}
			else if (event.type == Event::MouseWheelMoved) {
				int dpos = event.mouseWheel.delta;
				if (chat.getTyping()) {
					chat.scroll(dpos);
				}
			}
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Up) {
					if (chat.getTyping() && inFocus) {
						chat.goUp();
					}
				}
				if (event.key.code == Keyboard::Down) {
					if (chat.getTyping() && inFocus) {
						chat.goDown();
					}
				}
				if (event.key.code == Keyboard::Left) {
					if (chat.getTyping()) {
						if (!Keyboard::isKeyPressed(Keyboard::LControl) && !Keyboard::isKeyPressed(Keyboard::RControl)) {
							chat.mtx.lock();
							chat.moveCursor(-1);
							chat.mtx.unlock();
						}
						else chat.moveCursorCtrl(-1);
					}
				}
				if (event.key.code == Keyboard::Right) {
					if (chat.getTyping()) {
						if (!Keyboard::isKeyPressed(Keyboard::LControl) && !Keyboard::isKeyPressed(Keyboard::RControl)) {
							chat.mtx.lock();
							chat.moveCursor(1);
							chat.mtx.unlock();
						}
						else chat.moveCursorCtrl(1);
					}
				}
				if (event.key.code == Keyboard::Delete) {
					if (chat.getTyping()) {
						chat.deleteSymbol();
					}
				}
			}
		}
		if (inFocus && Keyboard::isKeyPressed(Keyboard::F11)) {
			if (!f11pressed) {
				fullscreen = !fullscreen;
				refreshWindow(window);
			}
			f11pressed = true;
		}
		else f11pressed = false;
		if (Keyboard::isKeyPressed(Keyboard::T)) {
			if (!tPressed && !chat.getTyping() && inFocus) {
				chat.setTyping(true);
			}
			tPressed = true;
		}
		else tPressed = false;
		if (Keyboard::isKeyPressed(Keyboard::Slash)) {
			if (!slashPressed && !chat.getTyping() && inFocus) {
				chat.setTyping(true);
				chat.putChar('/');
			}
			slashPressed = true;
		}
		else slashPressed = false;
		if (chat.getTyping() && inFocus && Keyboard::isKeyPressed(Keyboard::Escape)) {
			chat.setTyping(false);
		}
		if (Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (!enterPressed && chat.getTyping() && inFocus) {
				String curS = chat.getMessage();
				if (!curS.isEmpty()) {
					std::vector<String>cur;
					if (curS[0] == '/') {
						cur = parseCommand(curS.substring(1, (int)curS.getSize() - 1));
					}
					if (!cur.empty() && cur[0] == "connect") {
						closeGame = false;
						if (cur.size() == 1) {
							started = true;
						}
						else if (cur.size() == 2) {
							auto temp = parseIpPort(cur[1]);
							if (!temp.first.isEmpty()) {
								ip = (std::string)temp.first;
								port = temp.second;
								started = true;
							}
						}
					}
					else if (!cur.empty() && cur[0] == "disconnect") {
						closeGame = true;
					}
					else {
						Packet sPacket;
						sPacket << (Uint8)'/';
						sPacket << curS;
						sock.send(sPacket);
					}
				}
				chat.send();
				chat.setTyping(false);
			}
			enterPressed = true;
		}
		else enterPressed = false;
		if (!fullscreen) {
			windowX = window.getPosition().x;
			windowY = window.getPosition().y;
			if (window.isOpen() && (oldWindowX != windowX || oldWindowY != windowY)) {
				saveToSettings();
				oldWindowX = windowX;
				oldWindowY = windowY;
			}
		}
		mousex = Mouse::getPosition(window).x;
		mousey = Mouse::getPosition(window).y;
		window.clear(Color(255, 255, 255, 255));

		ld sum = 0;
		pingMtx.lock();
		for (auto i : pingPackets) {
			if (!i.second.second) {
				sum += gettime() - i.first;
			}
			else {
				sum += i.first;
			}
		}
		int pingPacketsSize = pingPackets.size();
		pingMtx.unlock();
		ping.setString(((pingPacketsSize == 0) ? "nan" : std::to_string((int)(sum * 1000 / pingPacketsSize))) + " ms");
		ping.setOrigin(getWidth(ping), getHeight(ping) + 7);
		ping.setPosition(WIDTH - 1, HEIGHT - 1);
		window.draw(ping);

		chat.draw(window, font);
		if (tankNum == -1) {
			window.display();
			continue;
		}
		frameTimes.push_back(gettime() - oldTime);
		oldTime = gettime();
		if (frameTimes.size() > 100)frameTimes.pop_front();
		sum = 0;
		for (auto i : frameTimes)sum += i;
		sum /= (ld)frameTimes.size();
		fps.setString(std::to_string((int)((ld)1 / sum)));
		fps.setOrigin(getWidth(fps), 0);
		fps.setPosition(WIDTH, 0);
		window.draw(fps);
		mtxD.lock();
		wMtx.lock();
		w.draw(window);
		wMtx.unlock();
		pMtx.lock();
		for (auto i : p)i.draw(window);
		pMtx.unlock();
		bMtx.lock();
		for (auto i : b)i.draw(window);
		bMtx.unlock();
		int curNum = -1;
		for (auto i : t) {
			curNum++;
			if (i.t.dead || tankNum == -1)continue;
			i.draw(window, (curNum == tankNum), timeSinceStart);
			if (i.t.bonus == 1 || i.t.bonus == 3 || i.t.bonus == 6) {
				ld tx = i.t.x + (TANK_LENGTH / 2 - BULLET_SIZE) * cosl(i.t.angle), ty = i.t.y + (TANK_LENGTH / 2 - BULLET_SIZE) * sinl(i.t.angle);
				ld curtime;
				if (i.t.bonus == 1) {
					curtime = ((curNum == tankNum) ? ((ld)BULLET_LIFETIME / 3) : ((ld)50 / BULLET_SPEED));
				}
				if (i.t.bonus == 3) {
					curtime = LASER_DISTANCE;
				}
				if (i.t.bonus == 6) {
					curtime = WALL_DESTROYER_DISTANCE;
				}
				ld curspeed = ((i.t.bonus == 1) ? BULLET_SPEED : 1);
				Color curcolor = ((i.t.bonus == 1) ? Color::Red : Color::Green);
				if (i.t.bonus == 1) {
					curcolor = Color::Red;
				}
				if (i.t.bonus == 3) {
					curcolor = Color::Green;
				}
				if (i.t.bonus == 6) {
					curcolor = Color::Yellow;
				}
				int curBounces = ((i.t.bonus == 3 || i.t.bonus == 1 && curNum != tankNum) ? -1 : BULLET_BOUNCES);
				if (i.t.bonus == 1) {
					curBounces = ((curNum != tankNum) ? -1 : BULLET_BOUNCES);
				}
				if (i.t.bonus == 3) {
					curBounces = -1;
				}
				if (i.t.bonus == 6) {
					curBounces = 0;
				}
				auto path = w.getTrajectory(tx, ty, i.t.angle, curspeed, curtime, curBounces);
				for (int i = 0; i < (int)path.size() - 1; i++) {
					drawLineSegment(window, path[i].second.x, path[i].second.y, path[i + 1].second.x, path[i + 1].second.y, curcolor);
				}
				if (i.t.bonus == 6) {
					CircleShape temp(WALL_DESTROYER_RADIUS);
					Color tempColor = Color::Yellow;
					tempColor.a = 191;
					temp.setFillColor(tempColor);
					temp.setOrigin(getWidth(temp) / 2, getHeight(temp) / 2);
					temp.setPosition(sx + path.back().second.x, sy + path.back().second.y);
					window.draw(temp);
				}
			}
		}
		lasersMtx.lock();
		for (auto i : lasers) {
			if (tankNum == -1)continue;
			const auto& path = i.path;
			Color curcolor(0, 255, 0, std::max(0, (int)((LASER_LIFETIME - i.timer) / LASER_LIFETIME * 256)));
			for (int i = 0; i < (int)path.size() - 1; i++) {
				drawLineSegment(window, path[i].second.x, path[i].second.y, path[i + 1].second.x, path[i + 1].second.y, curcolor, 3);
			}
		}
		lasersMtx.unlock();
		mtxD.unlock();
		std::vector<std::pair<int, std::string>>sortedPoints;
		for (auto i : t) {
			if (i.everConnected && i.points > 0 || i.connected) {
				sortedPoints.push_back(std::make_pair(i.points, i.nickname));
			}
		}
		std::sort(sortedPoints.rbegin(), sortedPoints.rend());
		String spoints = "Scoreboard\n";
		int curPlace = 1;
		for (int i = 0; i < sortedPoints.size(); i++) {
			if (i > 0 && sortedPoints[i - 1].first != sortedPoints[i].first)curPlace++;
			spoints += std::to_string(curPlace) + ". " + sortedPoints[i].second + " - " + std::to_string(sortedPoints[i].first) + "\n";
		}
		points.setString(spoints);
		window.draw(points);

		bcMtx.lock();
		int top = HEIGHT / 3;
		for (auto i : broadcastMessages) {
			broadcastText.setString(i.first);
			broadcastText.setCharacterSize(i.second.first);
			auto bcRect = broadcastText.getGlobalBounds();
			int cur = top;
			broadcastText.setPosition((WIDTH - bcRect.width) / 2, cur);
			top += bcRect.height + 20;
			broadcastText.setFillColor(Color(0, 0, 0, i.second.second));
			window.draw(broadcastText);
		}
		bcMtx.unlock();

		window.display();
	}
	windowOpen = false;
}
Keyboard::Key forwardsKey1 = Keyboard::Key::W;
Keyboard::Key backwardsKey1 = Keyboard::Key::S;
Keyboard::Key leftKey1 = Keyboard::Key::A;
Keyboard::Key rightKey1 = Keyboard::Key::D;
Keyboard::Key forwardsKey2 = Keyboard::Key::Up;
Keyboard::Key backwardsKey2 = Keyboard::Key::Down;
Keyboard::Key leftKey2 = Keyboard::Key::Left;
Keyboard::Key rightKey2 = Keyboard::Key::Right;
Keyboard::Key shootKey = Keyboard::Key::Space;
ld getAngle(ld a1, ld a2) {
	a1 = a2 - a1;
	int x = a1 / (2 * M_PI);
	a1 -= x * (2 * M_PI);
	if (a1 < -M_PI)a1 += 2 * M_PI;
	if (a1 > M_PI)a1 -= 2 * M_PI;
	return a1;
}
void checkAction(const walls& w, std::vector<bullet>& b, bool& shootKeyState, bool& mouseState, bool& oldShootKeyState, bool& oldMouseState) {
	if (shootKeyState) {
		oldShootKeyState = true;
	}
	else oldShootKeyState = false;
	if (mouseState) {
		oldMouseState = true;
	}
	else oldMouseState = false;
	shootKeyState = Keyboard::isKeyPressed(shootKey);
	mouseState = Mouse::isButtonPressed(Mouse::Left);
	if (inFocus && !shootKeyState && oldShootKeyState && !chat.getTyping()) {
		useKeyboard = true;
		saveToSettings();
	}
	if (inFocus && !mouseState && oldMouseState) {
		useKeyboard = false;
		saveToSettings();
	}
	t[tankNum].pressed = 0;
	if (useKeyboard) {
		if (inFocus && !chat.getTyping()) {
			t[tankNum].pressed += ((Keyboard::isKeyPressed(forwardsKey1) || Keyboard::isKeyPressed(forwardsKey2)) << 0);
			t[tankNum].pressed += ((Keyboard::isKeyPressed(backwardsKey1) || Keyboard::isKeyPressed(backwardsKey2)) << 1);
			t[tankNum].pressed += ((Keyboard::isKeyPressed(leftKey1) || Keyboard::isKeyPressed(leftKey2)) << 2);
			t[tankNum].pressed += ((Keyboard::isKeyPressed(rightKey1) || Keyboard::isKeyPressed(rightKey2)) << 3);
			t[tankNum].pressed += (shootKeyState << 4);
		}
	}
	else {
		if (inFocus) {
			ld dx = mousex - (t[tankNum].t.x * scale + sx);
			ld dy = mousey - (t[tankNum].t.y * scale + sy);
			ld ang = std::atan2(dy, dx);
			ld angle = getAngle(ang, t[tankNum].t.angle);
			if (dist(point(getdrawx(t[tankNum].t.x), getdrawy(t[tankNum].t.y)), point(mousex, mousey)) > 100) {
				t[tankNum].pressed += ((abs(angle) <= (2 * M_PI) / 8) << 0);
				t[tankNum].pressed += ((abs(abs(angle) - M_PI) <= (2 * M_PI) / 8) << 1);
			}
			if (dist(point(getdrawx(t[tankNum].t.x), getdrawy(t[tankNum].t.y)), point(mousex, mousey)) > 30 && !(t[tankNum].pressed & 2)) {
				t[tankNum].pressed += ((angle > (2 * M_PI) / 256) << 2);
				t[tankNum].pressed += ((angle < -(2 * M_PI) / 256) << 3);
			}
			t[tankNum].pressed += (mouseState << 4);
		}
	}
}
void game(Font gFont) {
	rc['S'].clear();

	auto connectRes = connect(Nickname, Password, sPassword);
	if (!connectRes.first) {
		chat.putMessage("Connected!");
		connected = true;
		timeSinceLastPacket = gettime();
	}
	if (connectRes.first == -1) {
		chat.putMessage("Couldn't connect to the server: Unexpected error.");
		started = false;
		return;
	}
	if (connectRes.first == 1) {
		chat.putMessage("Couldn't connect to the server: Server is not responding.");
		started = false;
		return;
	}
	if (connectRes.first == 2) {
		chat.putMessage("Couldn't connect to the server: " + connectRes.second + ".");
		started = false;
		return;
	}

	tankNum = -1;
	ld lastUpdated = gettime();
	ld sendTime = (ld)1 / PACKETS_PER_SECOND;
	ld lastSent = gettime();
	ld checkPacketTime = gettime();
	String dmsg = "disconnected";
	bool oldMouseState = false, oldShootKeyState = false, shootKeyState = false, mouseState = false;
	timeSinceStart = 0;
	ld lastTimeChanged = gettime();
	while (!closeGame) {
		if (!windowOpen)closeGame = true;
		rcMtx.lock();
		if (!rc['0'].empty()) {
			Packet received = rc['0'].front();
			rc['0'].pop_front();
			rcMtx.unlock();

			received >> GAMEMODE;
			received >> MOVEMENT_SPEED;
			received >> ROTATION_SPEED;
			received >> BULLET_SPEED;
			received >> BULLET_LIFETIME;
			received >> BULLET_BOUNCES;
			received >> RELOAD_TIME;

			int curTankNum;
			received >> curTankNum;

			received >> timeSinceStart;
			lastTimeChanged = gettime();

			bcMtx.lock();
			int temp = ceil((2 - timeSinceStart) / 2 * 3);
			broadcastMessages.clear();
			if (temp >= 1) {
				broadcastMessages.resize(2);
				broadcastMessages[0] = std::make_pair(std::to_string(temp), std::make_pair(100, 191));
				broadcastMessages[1] = std::make_pair(gamemodes_names[GAMEMODE], std::make_pair(50, 191));
			}
			else if (temp >= -2) {
				broadcastMessages.resize(2);
				broadcastMessages[0] = std::make_pair("GO!", std::make_pair(100, std::max(0, std::min(191, (int)((2 - timeSinceStart) * 191)))));
				broadcastMessages[1] = std::make_pair(gamemodes_names[GAMEMODE], std::make_pair(50, 0));
			}
			bcMtx.unlock();

			int sz;

			received >> mapSeed;
			received >> mapLength;
			received >> mapHeight;
			received >> sz;
			w.clear();
			for (int i = 0; i < sz; i++) {
				int x, y;
				bool vertical;
				received >> x;
				received >> y;
				received >> vertical;
				w.insert(wall(x, y, vertical));
			}

			buildMap();
			int old = PLAYER_COUNT;
			received >> PLAYER_COUNT;
			t.resize(PLAYER_COUNT);
			for (int i = old; i < PLAYER_COUNT; i++) {
				t[i].nicknameText.setFont(gFont);
				t[i].reloadText.setFont(gFont);
			}
			for (int i = 0; i < PLAYER_COUNT; i++) {
				received >> t[i].t.id;
				if (i != tankNum) {
					std::string temp;
					received >> temp;
					t[i].changeNickname(temp);
					received >> t[i].points;
					received >> t[i].t.dead;
					received >> t[i].connected;
					received >> t[i].everConnected;
					received >> t[i].pressed;
					received >> t[i].t.x;
					received >> t[i].t.y;
					received >> t[i].t.angle;
					received >> t[i].t.color;
					received >> t[i].t.bonus;
					received >> t[i].t.lastShot;
				}
				else {
					std::string trash1;
					int trash2;
					received >> trash1;
					received >> t[i].points;
					received >> t[i].t.dead;
					received >> t[i].connected;
					received >> t[i].everConnected;
					received >> trash2;
					received >> t[i].t.x;
					received >> t[i].t.y;
					received >> t[i].t.angle;
					received >> t[i].t.color;
					received >> t[i].t.bonus;
					received >> t[i].t.lastShot;
				}
			}
			received >> sz;
			bMtx.lock();
			b.clear();
			for (int i = 0; i < sz; i++) {
				ld startx, starty, startAngle;
				int uid;
				Uint8 type;
				ld timeCreated;
				received >> startx;
				received >> starty;
				received >> startAngle;
				received >> uid;
				received >> type;
				received >> timeCreated;
				b.push_back(bullet(startx, starty, startAngle, -1, uid, type, w, timeCreated));
				b.back().setTime(timeSinceStart);
			}
			bMtx.unlock();
			received >> sz;
			lasersMtx.lock();
			lasers.clear();
			for (int i = 0; i < sz; i++) {
				received >> lasers[i].x;
				received >> lasers[i].y;
				received >> lasers[i].angle;
				received >> lasers[i].uid;
			}
			lasersMtx.unlock();
			received >> sz;
			pMtx.lock();
			p.clear();
			for (int i = 0; i < sz; i++) {
				ld x, y;
				Uint8 type;
				int uid;
				received >> x;
				received >> y;
				received >> type;
				received >> uid;
				p.push_back(powerup(x, y, type, uid));
			}
			pMtx.unlock();
			tankNum = curTankNum;
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['U'].empty()) {
			Packet received = rc['U'].front();
			rc['U'].pop_front();
			rcMtx.unlock();

			ld temptimeSinceStart;
			received >> temptimeSinceStart;
			timeSinceStart = std::max(timeSinceStart + gettime() - lastTimeChanged, temptimeSinceStart);
			lastTimeChanged = gettime();

			bcMtx.lock();
			int temp = ceil((2 - timeSinceStart) / 2 * 3);
			broadcastMessages.clear();
			if (temp >= 1) {
				broadcastMessages.resize(2);
				broadcastMessages[0] = std::make_pair(std::to_string(temp), std::make_pair(100, 191));
				broadcastMessages[1] = std::make_pair(gamemodes_names[GAMEMODE], std::make_pair(50, 191));
			}
			else if (temp >= -2) {
				broadcastMessages.resize(2);
				broadcastMessages[0] = std::make_pair("GO!", std::make_pair(100, std::max(0, std::min(191, (int)((3 - timeSinceStart) * 191)))));
				broadcastMessages[1] = std::make_pair(gamemodes_names[GAMEMODE], std::make_pair(50, 0));
			}
			bcMtx.unlock();

			int sz;

			received >> sz;
			for (int i = 0; i < sz; i++) {
				int curid;
				received >> curid;
				while (curid >= t.size()) {
					t.push_back(player());
					t.back().nicknameText.setFont(gFont);
				}
				std::string temp;
				received >> temp;
				t[curid].changeNickname(temp);
				if (curid != tankNum) {
					received >> t[curid].points;
					received >> t[curid].t.dead;
					received >> t[curid].connected;
					received >> t[curid].everConnected;
					received >> t[curid].pressed;
					received >> t[curid].t.x;
					received >> t[curid].t.y;
					received >> t[curid].t.angle;
					received >> t[curid].t.color;
					received >> t[curid].t.bonus;
					received >> t[curid].t.lastShot;
				}
				else {
					std::string trash1;
					int trash2;
					ld trash3;
					received >> t[curid].points;
					received >> t[curid].t.dead;
					received >> t[curid].connected;
					received >> t[curid].everConnected;
					received >> trash2;
					received >> trash3;
					received >> trash3;
					received >> trash3;
					received >> t[curid].t.color;
					received >> t[curid].t.bonus;
					received >> t[curid].t.lastShot;
				}
			}
			received >> sz;
			wMtx.lock();
			for (int i = 0; i < sz; i++) {
				bool temp;
				int x, y;
				bool vertical;
				received >> temp;
				received >> x;
				received >> y;
				received >> vertical;
				wall cur(x, y, vertical);
				if (!temp) {
					w.insert(cur);
				}
				else {
					for (int i = 0; i < w.size(); i++) {
						if (w.w[i] == cur) {
							w.w.erase(w.w.begin() + i);
							break;
						}
					}
				}
			}
			if (sz) {
				w.build();
			}
			wMtx.unlock();
			received >> sz;
			bMtx.lock();
			std::vector<bullet>tb = b;
			bMtx.unlock();
			for (int i = 0; i < sz; i++) {
				bool temp;
				received >> temp;
				if (!temp) {
					ld startx, starty, startAngle;
					int uid;
					Uint8 type;
					ld timeCreated;
					received >> startx;
					received >> starty;
					received >> startAngle;
					received >> uid;
					received >> type;
					received >> timeCreated;
					tb.push_back(bullet(startx, starty, startAngle, -1, uid, type, w, timeCreated));
					tb.back().setTime(timeSinceStart);
				}
				else {
					int uid;
					received >> uid;
					for (int j = 0; j < tb.size(); j++) {
						if (tb[j].uid == uid) {
							tb.erase(tb.begin() + j--);
						}
					}
				}
			}
			for (auto& i : tb) {
				i.setTime(timeSinceStart);
			}
			bMtx.lock();
			b = tb;
			bMtx.unlock();
			received >> sz;
			lasersMtx.lock();
			for (int i = 0; i < sz; i++) {
				ld x, y, angle;
				int uid;
				received >> x;
				received >> y;
				received >> angle;
				received >> uid;
				lasers.push_back(laser(x, y, angle, uid, w));
			}
			lasersMtx.unlock();
			received >> sz;
			pMtx.lock();
			std::vector<powerup>tp = p;
			pMtx.unlock();
			for (int i = 0; i < sz; i++) {
				bool temp;
				received >> temp;
				if (!temp) {
					ld x, y;
					Uint8 type;
					int uid;
					received >> x >> y >> type >> uid;
					tp.push_back(powerup(x, y, type, uid));
				}
				else {
					int uid;
					received >> uid;
					for (int j = 0; j < tp.size(); j++) {
						if (tp[j].uid == uid) {
							tp.erase(tp.begin() + j--);
						}
					}
				}
			}
			pMtx.lock();
			p = tp;
			pMtx.unlock();
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['P'].empty()) {
			Packet received = rc['P'].front();
			rc['P'].pop_front();
			rcMtx.unlock();
			timeSinceLastPacket = gettime();
			int num;
			received >> num;
			pingMtx.lock();
			for (int i = 0; i < pingPackets.size(); i++) {
				if (pingPackets[i].second.first == num && !pingPackets[i].second.second) {
					pingPackets[i].second.second = true;
					pingPackets[i].first = gettime() - pingPackets[i].first;
					break;
				}
			}
			pingMtx.unlock();
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['D'].empty()) {
			Packet received = rc['D'].front();
			rc['D'].pop_front();
			rcMtx.unlock();
			received >> dmsg;
			Packet disconnectPacket;
			disconnectPacket << (Uint8)'D' << dmsg;
			sock.send(disconnectPacket);
			chat.putMessage("Disconnected: " + dmsg + ".");
			pingMtx.lock();
			pingPackets.clear();
			pingMtx.unlock();
			break;
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['B'].empty()) {
			Packet received = rc['B'].front();
			rc['B'].pop_front();
			String s;
			received >> s;
			rcMtx.unlock();
			chat.putMessage(s);
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['L'].empty()) {
			Packet received = rc['L'].front();
			rc['L'].pop_front();
			rcMtx.unlock();
			listMtx.lock();
			ncList.clear();
			int sz;
			received >> sz;
			ncList.resize(sz);
			for (int i = 0; i < sz; i++) {
				std::string cur;
				received >> cur;
				ncList[i] = cur;
			}
			listMtx.unlock();
		}
		else rcMtx.unlock();
		if (gettime() - lastSent > sendTime) {
			lastSent = gettime();
			if (gettime() - timeSinceLastPacket > DISCONNECT_AFTER) {
				dmsg = "timed out";
				Packet disconnectPacket;
				disconnectPacket << (Uint8)'D' << dmsg;
				sock.send(disconnectPacket);
				chat.putMessage("Disconnected: " + dmsg + ".");
				pingMtx.lock();
				pingPackets.clear();
				pingMtx.unlock();
				break;
			}
			if (timeSinceStart >= 2 && tankNum != -1) {
				Packet state;
				state << (Uint8)'S';
				state << mapSeed;
				state << t[tankNum].pressed;
				state << t[tankNum].t.x;
				state << t[tankNum].t.y;
				state << t[tankNum].t.angle;
				sock.send(state);
			}
		}
		if (gettime() - lastUpdated > UPDATE_TIME) {
			ld dt = gettime() - lastUpdated;
			lastUpdated = gettime();
			if (tankNum != -1) {
				timeSinceStart += gettime() - lastTimeChanged;
				lastTimeChanged = gettime();
				checkAction(w, b, shootKeyState, mouseState, oldShootKeyState, oldMouseState);
				if (timeSinceStart >= 2)for (auto& i : t)i.checkAction(w, dt);
				bMtx.lock();
				for (auto& i : b)i.setTime(timeSinceStart);
				bMtx.unlock();
				for (int i = 0; i < lasers.size(); i++) {
					lasers[i].update(dt);
					if (lasers[i].timer >= LASER_LIFETIME) {
						lasers.erase(lasers.begin() + i--);
					}
				}
			}
		}
		if (gettime() - checkPacketTime > 0.25) {
			checkPacketTime = gettime();
			Packet sPacket;
			sPacket << (Uint8)'P';
			int num = rnd();
			sPacket << num;
			sock.send(sPacket);
			pingMtx.lock();
			pingPackets.push_back(std::make_pair(gettime(), std::make_pair(num, false)));
			while (pingPackets.size() && gettime() - pingPackets.front().first >= 10)pingPackets.pop_front();
			pingMtx.unlock();
		}
	}
	if (closeGame) {
		dmsg = "left the server";
		Packet disconnectPacket;
		disconnectPacket << (Uint8)'D' << dmsg;
		sock.send(disconnectPacket);
		Sleep(100);
		chat.putMessage("Disconnected: " + dmsg + ".");
		pingMtx.lock();
		pingPackets.clear();
		pingMtx.unlock();
	}
	sock.disconnect();
	started = false;
	rcMtx.lock();
	rc['D'].clear();
	rcMtx.unlock();
	tankNum = -1;
}
int main() {
	Texture Tank[3];
	for (int i = 0; i < 3; i++) {
		Tank[i].loadFromFile("Resources\\Tank" + std::to_string(i) + ".png");
		Tank[i].setSmooth(true);
		TankSprite[i].setTexture(Tank[i]);
	}
	Texture Bonus[7];
	for (int i = 1; i <= 6; i++) {
		Bonus[i].loadFromFile("Resources\\Bonus" + std::to_string(i) + ".jpg");
		Bonus[i].setSmooth(true);
		BonusSprite[i].setTexture(Bonus[i]);
	}
	Font gFont;
	gFont.loadFromFile("Resources\\font.ttf");

	std::vector<std::pair<std::string, std::string>>parsed;
	std::map<std::string, std::string>settings;
	{
		std::ifstream in("settings.txt");
		if (!in.good()) {
			in.close();
			std::ofstream out("settings.txt");
			out.close();
			in = std::ifstream("settings.txt");
		}
		parsed = parseInput(in);
		for (auto i : parsed)settings[i.first] = i.second;
	}
	Nickname = settings["nickname"];
	if (Nickname == "") {
		Password = "";
		Nickname = std::to_string(randnum(1, 1e9));
	}
	Password = settings["password"];
	sPassword = settings["server_password"];
	if (isNum(settings["window_length"]))windowLength = std::stoi(settings["window_length"]);
	else windowLength = 800;
	if (isNum(settings["window_height"]))windowHeight = std::stoi(settings["window_height"]);
	else windowHeight = 600;
	if (isNum(settings["window_Xpos"]))windowX = std::stoi(settings["window_Xpos"]);
	else windowX = (VideoMode::getFullscreenModes()[0].width - windowLength) / 2;
	if (isNum(settings["window_Ypos"]))windowY = std::stoi(settings["window_Ypos"]);
	else windowY = (VideoMode::getFullscreenModes()[0].height - windowHeight) / 2;
	if (isNum(settings["fullscreen_mode"]))fullscreen = std::stoi(settings["fullscreen_mode"]);
	else fullscreen = false;
	if (isNum(settings["use_keyboard"]))useKeyboard = std::stoi(settings["use_keyboard"]);
	else useKeyboard = false;

	WIDTH = windowLength;
	HEIGHT = windowHeight;

	windowOpen = true;
	std::thread drawingThread(drawing);

	std::thread pThread(handlingPackets);

	started = true;

	while (windowOpen) {
		if (started) {
			game(gFont);
		}
		Sleep(1);
	}

	handling = false;
	pThread.detach();
	drawingThread.join();
	return 0;
}