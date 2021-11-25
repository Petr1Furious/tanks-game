//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
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
#include<sstream>
#include<fstream>
#include<thread>
#include<random>
#include<chrono>
#include<thread>
#include<deque>
#include<cmath>
#include<mutex>
#include<queue>
#include<map>
#include<set>
#define CONNECT_TIME 1000
#define PACKETS_PER_SECOND 40
#define PLAYERS_COUNT 10
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
bool isNum(String s) {
	if (!s.isEmpty() && s[0] == '-')s.erase(0);
	if (s.getSize() == 0 || s[0] == '0' && s.getSize() != 1)return false;
	for (auto i : s) {
		if (!('0' <= i && i <= '9'))return false;
	}
	return true;
}
bool is16Num(String s) {
	if (!s.isEmpty() && s[0] == '-')s.erase(0);
	if (s.getSize() == 0)return false;
	for (auto i : s) {
		if (!('0' <= i && i <= '9') && !(('A' <= i && i <= 'F') || ('a' <= i && i <= 'f')))return false;
	}
	return true;
}
int GAMEMODE;
ld MOVEMENT_SPEED = 250;
ld ROTATION_SPEED = 270;
#define WALL_LENGTH 100
#define WALL_THICKNESS 4
#define TANK_LENGTH 38
#define TANK_WIDTH 28
#define BARREL_SIZE 7
#define BULLET_SIZE 3
ld BULLET_SPEED = 400;
ld BULLET_LIFETIME = 15;
int BULLET_BOUNCES = -1;
ld RELOAD_TIME = 1;
#define UPDATE_TIME (ld)0.003
#define BORDER_SIZE 10
#define MAX_BULLET_COUNT 5
#define TIME_BEFORE_RESTART 5
#define POWERUP_SIZE 30
#define POWERUP_TIME 15
#define INVISIBILITY_TIME 5
#define LASER_DISTANCE 1000
#define LASER_LIFETIME 0.5
#define BULLET_PARTS_COUNT 20
#define SHARD_SPEED 800
#define WALL_DESTROYER_DISTANCE 100
#define WALL_DESTROYER_RADIUS 10
int PERMANENT_POWERUP = 0;
std::vector<int>availablePowerups;
struct wall {
	int x, y;
	bool vertical;
	RectangleShape rect;
	std::vector<point>v;
	bool boundary;
	wall(int _x, int _y, bool _vertical, bool _boundary) :x(_x), y(_y), vertical(_vertical), boundary(_boundary) {
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
	bool operator==(const wall& w)const {
		return x == w.x && y == w.y && vertical == w.vertical && boundary == w.boundary;
	}
	void loadToSend(Packet& state)const {
		state << x;
		state << y;
		state << vertical;
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
	walls() :mxx(0), mxy(0) {}
	void insert(const wall& c) {
		w.push_back(c);
	}
	int size() {
		return w.size();
	}
	void clear() {
		w.clear();
		wx.clear();
		wy.clear();
		mxx = 0;
		mxy = 0;
	}
	void eraseRandom(int cnt) {
		for (int i = 0; i < cnt; i++) {
			w.erase(w.begin() + Mrandnum(0, (int)w.size() - 1));
		}
	}
	void build() {
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
	std::vector<std::pair<ld, point>>getTrajectory(ld x, ld y, ld angle, ld speed, ld lifetime, int bounces)const {
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
		return path;
	}
	void addh(int i, int j, bool boundary) {
		int x = ((i == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * i));
		int y = ((j == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * j));
		insert(wall(x + WALL_LENGTH / 2, y + WALL_THICKNESS / 2, false, boundary));
	}
	void addv(int i, int j, bool boundary) {
		int x = ((i == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * i));
		int y = ((j == 0) ? 0 : ((WALL_LENGTH - WALL_THICKNESS) * j));
		insert(wall(x + WALL_THICKNESS / 2, y + WALL_LENGTH / 2, true, boundary));
	}
};
struct bullet {
	ld x, y, angle;
	ld startx, starty, startAngle;
	ld timer;
	int id, uid;
	bool everBounced;
	bool firstBounce;
	std::vector<std::pair<ld, point>>path;
	int pos;
	ld creationTime;
	Uint8 type;
	std::vector<std::pair<point, point>>lastMovement;
	ld speed;
	int bounces;
	bullet(ld _x, ld _y, ld _angle, int _id, Uint8 _type, const walls& w, ld minusTime = 0, int minusBounces = 0) :x(_x), y(_y), timer(0), id(_id), type(_type), everBounced(false), firstBounce(false), pos(0) {
		creationTime = gettime();
		startx = _x;
		starty = _y;
		startAngle = _angle;
		angle = startAngle;
		id = _id;
		lastMovement.push_back(std::make_pair(point(x, y), point(x, y)));
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
		path = w.getTrajectory(startx, starty, startAngle, speed, BULLET_LIFETIME - minusTime, curBounces - ((curBounces == -1) ? 0 : minusBounces));
	}
	void move(ld dt) {
		timer += dt;
		int oldPos = pos + (pos == 0);
		while (pos < path.size() && path[pos].first < timer) {
			pos++;
			bounces++;
		}
		if (pos >= 2) {
			everBounced = true;
			if (!firstBounce) {
				firstBounce = true;
			}
			else {
				firstBounce = false;
			}
		}
		lastMovement.clear();
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
		for (int i = oldPos; i <= pos; i++) {
			if (i >= path.size())break;
			point f;
			if (i == oldPos) {
				f = path[i].second - path[i - 1].second;
				f = f / f.len() * (((timer - dt) - path[i - 1].first) * speed);
				f = f + path[i - 1].second;
			}
			else {
				f = path[i - 1].second;
			}
			point s;
			if (i == pos) {
				s = path[i].second - path[i - 1].second;
				s = s / s.len() * ((timer - path[i - 1].first) * speed);
				s = s + path[i - 1].second;
			}
			else {
				s = path[i].second;
			}
			lastMovement.push_back(std::make_pair(f, s));
		}
	}
	void loadToSend(Packet& state, ld gameTime) {
		state << startx;
		state << starty;
		state << startAngle;
		state << uid;
		state << type;
		state << creationTime - gameTime;
	}
};
struct powerup {
	ld x, y;
	Uint8 type;
	int uid;
	std::vector<point>box;
	powerup(ld _x, ld _y, Uint8 _type) : x(_x), y(_y), type(_type) {
		box.push_back(point(x - POWERUP_SIZE / 2, y - POWERUP_SIZE / 2));
		box.push_back(point(x - POWERUP_SIZE / 2, y + POWERUP_SIZE / 2));
		box.push_back(point(x + POWERUP_SIZE / 2, y - POWERUP_SIZE / 2));
		box.push_back(point(x + POWERUP_SIZE / 2, y + POWERUP_SIZE / 2));
	}
	void loadToSend(Packet& state) {
		state << x;
		state << y;
		state << type;
		state << uid;
	}
};
struct laser {
	ld x, y, angle;
	ld id;
	int uid;
	std::vector<std::pair<ld, point>>path;
	laser(ld _x, ld _y, ld _angle, ld _id, const walls& w) :x(_x), y(_y), angle(_angle), id(_id) {
		path = w.getTrajectory(x, y, angle, 1, LASER_DISTANCE, -1);
	}
	void loadToSend(Packet& state) {
		state << x;
		state << y;
		state << angle;
		state << uid;
	}
};
std::vector<std::pair<bool, int>>recentlyChangedBullets;
int curuidb = 0;
void addBullet(std::vector<bullet>& b, bullet curb) {
	curb.uid = curuidb++;
	b.push_back(curb);
	recentlyChangedBullets.push_back({ false, curb.uid });
}
void destroyBullet(std::vector<bullet>& b, const walls& w, int id) {
	bool flag = b[id].type == 1;
	for (int i = 0; i < recentlyChangedBullets.size(); i++) {
		if (recentlyChangedBullets[i].second == b[id].uid) {
			recentlyChangedBullets.erase(recentlyChangedBullets.begin() + i);
		}
	}
	recentlyChangedBullets.push_back({ true, b[id].uid });
	if (flag) {
		for (int i = 0; i < BULLET_PARTS_COUNT; i++) {
			addBullet(b, bullet(b[id].x, b[id].y, 2 * M_PI * randnum(0, 1e9 - 1) / 1e9, b[id].id, 2, w));
		}
	}
	b.erase(b.begin() + id);
}
std::vector<std::pair<bool, int>>recentlyChangedPowerups;
int curuidp = 0;
void addPowerup(std::vector<powerup>& p, powerup curp) {
	curp.uid = curuidp++;
	p.push_back(curp);
	recentlyChangedPowerups.push_back({ false, curp.uid });
}
void destroyPowerup(std::vector<powerup>& p, int id) {
	for (int i = 0; i < recentlyChangedPowerups.size(); i++) {
		if (recentlyChangedPowerups[i].second == p[id].uid) {
			recentlyChangedPowerups.erase(recentlyChangedPowerups.begin() + i);
		}
	}
	recentlyChangedPowerups.push_back({ true, p[id].uid });
	p.erase(p.begin() + id);
}
int curuidl = 0;
void addLaser(std::vector<laser>& l, laser curl) {
	curl.uid = curuidl++;
	l.push_back(curl);
}
std::vector<std::pair<bool, wall>>recentlyChangedWalls;
void addWall(walls& w, wall cur) {
	recentlyChangedWalls.push_back({ false, cur });
}
void destroyWall(walls& w, wall cur) {
	for (int i = 0; i < w.size(); i++) {
		if (w.w[i] == cur) {
			w.w.erase(w.w.begin() + i);
			recentlyChangedWalls.push_back({ true, cur });
			break;
		}
	}
}
struct tank {
	ld x, y, angle;
	int id;
	bool dead;
	int color;
	Uint8 bonus;
	ld bonusPickedUp;
	ld lastShot;
	tank() : x(0), y(0), angle(0), dead(true), color(Color(255, 255, 255, 255).toInteger()), bonus(0), lastShot(gettime()) {}
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
	void shoot(std::vector<bullet>& b, std::vector<laser>& lasers, walls& w) {
		if (dead)return;
		int cnt = 0;
		int found = -1;
		for (int i = 0; i < b.size(); i++) {
			if (b[i].id == id) {
				if (b[i].type != 2)cnt++;
				if (b[i].type == 1) {
					found = i;
				}
			}
		}
		if (found != -1) {
			destroyBullet(b, w, found);
		}
		else if (cnt < MAX_BULLET_COUNT || bonus == 3) {
			ld dx = (TANK_LENGTH / 2 - BULLET_SIZE) * cosl(angle), dy = (TANK_LENGTH / 2 - BULLET_SIZE) * sinl(angle);
			if (gettime() - lastShot >= RELOAD_TIME) {
				Uint8 type = 0;
				if (bonus == 4) {
					type = 1;
				}
				if (bonus == 5) {
					type = 3;
				}
				if (bonus == 0 || bonus == 1 || bonus == 2 || bonus == 4 || bonus == 5) {
					addBullet(b, bullet(x + dx, y + dy, angle, id, type, w));
				}
				if (bonus == 3) {
					addLaser(lasers, laser(x + dx, y + dy, angle, id, w));
				}
				if (bonus == 6) {
					auto path = w.getTrajectory(x + dx, y + dy, angle, 1, WALL_DESTROYER_DISTANCE, 0);
					bool changed = false;
					for (int i = 0; i < w.size(); i++) {
						if (dist(w.w[i].v, path.back().second) < WALL_DESTROYER_RADIUS && !w.w[i].boundary) {
							destroyWall(w, w.w[i--]);
							changed = true;
						}
					}
					if (changed) {
						w.build();
						auto temp = b;
						for (int i = 0; i < b.size(); i++) {
							destroyBullet(b, w, i--);
						}
						for (auto i : temp) {
							addBullet(b, bullet(i.x, i.y, i.angle, i.id, i.type, w, gettime() - i.creationTime, i.bounces));
						}
					}
				}
				lastShot = gettime();
				if (bonus == 1 || bonus == 2 || bonus == 3 || bonus == 4 || bonus == 5 || bonus == 6)bonus = PERMANENT_POWERUP;
			}
		}
	}
	bool collided(bullet& i) {
		if (dead)return false;
		std::vector<point>rect = getRect();
		int found = -1;
		int counter = 0;
		for (auto segment : i.lastMovement) {
			if (intersect(segment.first, segment.second, rect) || isIn(rect, segment.first)) {
				found = counter;
			}
			counter++;
		}
		if (i.type == 2)return found != -1;
		if (id != i.id)return found != -1;
		if (found == -1)return false;
		return i.everBounced && !(i.firstBounce && found == 0);
	}
	int collided(std::vector<bullet>& b) {
		if (dead)return -1;
		for (int i = 0; i < b.size(); i++)if (collided(b[i]))return i;
		return -1;
	}
	bool collided(const laser& l) {
		for (int i = 0; i < (int)l.path.size() - 1; i++) {
			if (intersect(l.path[i].second, l.path[i + 1].second, getRect()) && !(id == l.id && i == 0)) {
				return true;
			}
		}
		return false;
	}
	int collided(const std::vector<laser>& lasers) {
		for (int i = 0; i < lasers.size(); i++) {
			if (collided(lasers[i]))return i;
		}
		return -1;
	}
	bool collided(const powerup& i) {
		return intersect(getRect(), i.box);
	}
	int collided(const std::vector<powerup>& p) {
		for (int i = 0; i < p.size(); i++)if (collided(p[i]))return i;
		return -1;
	}
	void remove() {
		dead = true;
	}
	bool operator==(const tank& s)const {
		return x == s.x && y == s.y && angle == s.angle && id == s.id && dead == s.dead && color == s.color && bonus == s.bonus && lastShot == s.lastShot;
	}
	bool operator!=(const tank& s)const {
		return !(*this == s);
	}
};
struct player {
	tank t;
	int pressed;
	std::string nickname;
	bool connected, everConnected;
	IpAddress ip;
	unsigned int port;
	int points;
	bool lastPressed;
	ld timeSinceLastPacket;
	player() :pressed(0), nickname(""), connected(false), everConnected(false), points(0), lastPressed(false), timeSinceLastPacket(gettime()) {}
	void connect(IpAddress _ip, unsigned int _port, std::string _nickname) {
		connected = true;
		everConnected = true;
		ip = _ip;
		port = _port;
		nickname = _nickname;
	}
	void disconnect() {
		connected = false;
	}
	void checkAction(walls& w, std::vector<bullet>& b, std::vector<laser>& lasers, ld dt) {
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
		if (pressed & 16) {
			if (!lastPressed)t.shoot(b, lasers, w);
			lastPressed = true;
		}
		else lastPressed = false;
	}
	void loadToSend(Packet& state, ld gameTime) {
		state << t.id;
		state << nickname;
		state << points;
		state << t.dead;
		state << connected;
		state << everConnected;
		state << pressed;
		state << t.x;
		state << t.y;
		state << t.angle;
		state << t.color;
		state << t.bonus;
		state << t.lastShot - gameTime;
	}
	bool operator==(const player& s)const {
		return t == s.t && pressed == s.pressed && nickname == s.nickname && connected == s.connected && everConnected == s.everConnected && ip == s.ip && port == s.port && points == s.points;
	}
	bool operator!=(const player& s)const {
		return !(*this == s);
	}
};
Font font;
std::vector<player>t;
std::string Password;
walls w;
std::vector<bullet>b;
std::vector<powerup>p;
std::vector<laser>lasers;
int mapLength, mapHeight;
int mapSeed;
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
struct dsu {
	std::vector<int>p;
	void build(int x) {
		p.resize(x);
		for (int i = 0; i < x; i++)p[i] = i;
	}
	int get(int v) {
		return ((p[v] == v) ? v : (p[v] = get(p[v])));
	}
	void unite(int v, int u) {
		if (randnum(0, 1))std::swap(v, u);
		p[get(v)] = get(u);
	}
};
void kruskal(std::vector<std::vector<bool>>& hwall, std::vector<std::vector<bool>>& vwall) {
	dsu d;
	d.build(mapLength * mapHeight);
	std::vector<std::pair<int, int>>edges;
	for (int i = 0; i < mapLength; i++) {
		for (int j = 0; j < mapHeight; j++) {
			if (i < mapLength - 1) {
				edges.push_back(std::make_pair(i * mapHeight + j, (i + 1) * mapHeight + j));
			}
			if (j < mapHeight - 1) {
				edges.push_back(std::make_pair(i * mapHeight + j, i * mapHeight + j + 1));
			}
		}
	}
	std::shuffle(edges.begin(), edges.end(), rnd);
	for (auto edge : edges) {
		if (d.get(edge.first) != d.get(edge.second)) {
			d.unite(edge.first, edge.second);
			if (edge.first + mapHeight == edge.second) {
				vwall[edge.first / mapHeight][edge.first % mapHeight] = false;
			}
			else {
				hwall[edge.first / mapHeight][edge.first % mapHeight] = false;
			}
		}
	}
}
void buildMap(int mapType) {
	mapBuilder = std::mt19937(mapSeed);
	mapLength = Mrandnum(1, 18);
	mapHeight = Mrandnum(1, 10);
	std::vector<std::vector<bool>>hwall(mapLength, std::vector<bool>(mapHeight - 1, true));
	std::vector<std::vector<bool>>vwall(mapLength - 1, std::vector<bool>(mapHeight, true));
	if (mapType == 0) {
		std::vector<std::vector<bool>>used(mapLength, std::vector<bool>(mapHeight, false));
		dfs(Mrandnum(0, mapLength - 1), Mrandnum(0, mapHeight - 1), used, hwall, vwall);
	}
	else if (mapType == 1) {
		kruskal(hwall, vwall);
	}
	w.clear();
	for (int i = 0; i < mapLength; i++) {
		for (int j = 0; j < mapHeight; j++) {
			if (i != 0 && vwall[i - 1][j]) {
				w.addv(i, j, false);
			}
			if (i != mapLength - 1 && vwall[i][j]) {
				w.addv(i + 1, j, false);
			}
			if (j != 0 && hwall[i][j - 1]) {
				w.addh(i, j, false);
			}
			if (j != mapHeight - 1 && hwall[i][j]) {
				w.addh(i, j + 1, false);
			}
		}
	}
	w.eraseRandom(w.size() / 3);
	for (int i = 0; i < mapLength; i++) {
		for (int j = 0; j < mapHeight; j++) {
			if (i == 0) {
				w.addv(i, j, true);
			}
			if (i == mapLength - 1) {
				w.addv(i + 1, j, true);
			}
			if (j == 0) {
				w.addh(i, j, true);
			}
			if (j == mapHeight - 1) {
				w.addh(i, j + 1, true);
			}
		}
	}
	w.build();
}
template<class T>
float getWidth(T& t) {
	auto temp = t.getLocalBounds();
	return temp.width;
}
template<class T>
float getHeight(T& t) {
	auto temp = t.getLocalBounds();
	return temp.height;
}
template<class T>
void centerDownOrigin(T& t) {
	t.setOrigin((getLength(t) - 1) / 2, getHeight(t) - 1);
}
template <class T>
std::vector<unsigned char>toBytes(const T& data) {
	unsigned char* bytes = ((unsigned char*)(&data));
	std::vector<unsigned char> output;
	for (int i = 0; i < sizeof(data); i++)output.push_back(bytes[i]);
	return output;
}
template <class T>
void fromBytes(const std::vector<unsigned char>& input_bytes, T& val) {
	unsigned char* bytes = new unsigned char[input_bytes.size()];
	for (int i = 0; i < (int)input_bytes.size(); i++)bytes[i] = input_bytes[i];
	val = ((T&)(*bytes));
}
struct IpPortPacket {
	Packet packet;
	IpAddress ip;
	unsigned short port;
	IpPortPacket() {}
	IpPortPacket(Packet _packet, IpAddress _ip, unsigned short _port) :packet(_packet), ip(_ip), port(_port) {}
};
SocketSelector selector;
std::mutex rcMtx;
std::deque<IpPortPacket>rc[256];
std::mutex clientsMtx;
std::pair<std::pair<TcpSocket, ld>, bool>clients[PLAYERS_COUNT];
bool handling = true;

bool running = true;
void handlingPackets() {
	TcpListener listener;
	if (listener.listen(13111) != Socket::Done) {
		std::cout << "Failed to bind port!\nPlease restart server.";
		handling = false;
		return;
	}
	selector.add(listener);
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		clients[i].first.second = gettime() - DISCONNECT_AFTER - 1;
		clients[i].second = false;
	}
	while (handling) {
		if (selector.wait(seconds(0.1))) {
			if (selector.isReady(listener)) {
				clientsMtx.lock();
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					if (!clients[i].second && gettime() - clients[i].first.second >= DISCONNECT_AFTER) {
						clients[i].first.first.disconnect();
						listener.accept(clients[i].first.first);
						clients[i].first.second = gettime();
						clients[i].second = false;
						selector.add(clients[i].first.first);
						break;
					}
				}
				clientsMtx.unlock();
			}
			else {
				clientsMtx.lock();
				for (auto& cur : clients) {
					if (selector.isReady(cur.first.first)) {
						Packet rPacket;
						if (cur.first.first.receive(rPacket) == Socket::Disconnected) {
							selector.remove(cur.first.first);
							cur.first.first.disconnect();
							cur.first.second = gettime() - DISCONNECT_AFTER - 1;
							cur.second = false;
							continue;
						}
						Uint8 header;
						rPacket >> header;
						rcMtx.lock();
						rc[header].push_back(IpPortPacket(rPacket, cur.first.first.getRemoteAddress(), cur.first.first.getRemotePort()));
						if (rc[header].size() > PACKET_STORAGE_SIZE) {
							rc[header].pop_back();
						}
						rcMtx.unlock();
					}
				}
				clientsMtx.unlock();
			}
		}
	}
}
void send(Packet packet, IpAddress ip, unsigned short port) {
	clientsMtx.lock();
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (clients[i].first.first.getRemoteAddress() == ip && clients[i].first.first.getRemotePort() == port) {
			clients[i].first.first.send(packet);
		}
	}
	clientsMtx.unlock();
}
int newPlayerPos = 0;
bool oldConnected;
IpAddress oldIp;
unsigned short oldPort;
struct playerdata {
	std::string password;
	int kills;
	int deaths;
	int wins;
	int losses;
	Uint32 color;
	bool canOperate;
	bool isBanned;
	playerdata() {
		password = "";
		kills = 0;
		deaths = 0;
		wins = 0;
		losses = 0;
		color = Color(255, 0, 0, 255).toInteger();
		canOperate = false;
		isBanned = false;
	}
	playerdata(std::string _password) {
		password = _password;
		kills = 0;
		deaths = 0;
		wins = 0;
		losses = 0;
		color = Color(255, 0, 0, 255).toInteger();
		canOperate = false;
		isBanned = false;
	}
	playerdata(std::string _password, int _kills, int _deaths, int _wins, int _losses, Uint32 _color, Uint8 _canOperate, Uint8 _isBanned) {
		password = _password;
		kills = _kills;
		deaths = _deaths;
		wins = _wins;
		losses = _losses;
		color = _color;
		canOperate = _canOperate;
		isBanned = _isBanned;
	}
};
struct data {
	std::map<std::string, playerdata>m;
	void load() {
		std::ifstream in("database");
		if (!in.good()) {
			in.close();
			std::ofstream temp("database");
			temp.close();
			in = std::ifstream("database");
		}
		std::string nickname, password, kills, deaths, wins, losses, color, canOperate, isBanned;
		while (in >> nickname) {
			std::string temp;
			in >> temp;
			int sz = std::stoi(temp);
			if (sz)in >> password;
			else password = "";
			in >> kills >> deaths >> wins >> losses >> color >> canOperate >> isBanned;
			m[nickname] = playerdata(password, std::stoi(kills), std::stoi(deaths), std::stoi(wins), std::stoi(losses), std::stoull(color), std::stoi(canOperate), std::stoi(isBanned));
		}
	}
	void saveDataToFile() {
		std::ofstream out("database");
		for (auto i : m) {
			out << i.first << ' ';
			out << (int)i.second.password.size() << ' ';
			out << i.second.password << ' ';
			out << i.second.kills << ' ';
			out << i.second.deaths << ' ';
			out << i.second.wins << ' ';
			out << i.second.losses << ' ';
			out << i.second.color << ' ';
			out << i.second.canOperate << ' ';
			out << i.second.isBanned << '\n';
		}
		out.close();
	}
	bool find(std::string nickname) {
		return m.find(nickname) != m.end();
	}
	void createUser(std::string username, playerdata pd = playerdata()) {
		m[username] = pd;
		saveDataToFile();
	}
	std::string getPassword(std::string nickname) {
		return m[nickname].password;
	}
	void setPassword(std::string nickname, std::string _password) {
		m[nickname].password = _password;
		saveDataToFile();
	}
	bool getOp(std::string nickname) {
		return m[nickname].canOperate;
	}
	void op(std::string nickname) {
		m[nickname].canOperate = true;
		saveDataToFile();
	}
	void deop(std::string nickname) {
		m[nickname].canOperate = false;
		saveDataToFile();
	}
	int getKills(std::string nickname) {
		return m[nickname].kills;
	}
	void setKills(std::string nickname, int _kills) {
		m[nickname].kills = _kills;
		saveDataToFile();
	}
	int getDeaths(std::string nickname) {
		return m[nickname].deaths;
	}
	void setDeaths(std::string nickname, int _deaths) {
		m[nickname].deaths = _deaths;
		saveDataToFile();
	}
	int getWins(std::string nickname) {
		return m[nickname].wins;
	}
	void setWins(std::string nickname, int _wins) {
		m[nickname].wins = _wins;
		saveDataToFile();
	}
	int getLosses(std::string nickname) {
		return m[nickname].losses;
	}
	void setLosses(std::string nickname, int _losses) {
		m[nickname].losses = _losses;
		saveDataToFile();
	}
	Uint32 getColor(std::string nickname) {
		return m[nickname].color;
	}
	void setColor(std::string nickname, Uint32 _color) {
		m[nickname].color = _color;
		saveDataToFile();
	}
	Uint8 getBan(std::string nickname) {
		return m[nickname].isBanned;
	}
	void ban(std::string nickname) {
		m[nickname].isBanned = true;
		saveDataToFile();
	}
	void unban(std::string nickname) {
		m[nickname].isBanned = false;
		saveDataToFile();
	}
};
data db;
bool connect(std::string nickname, std::string password, std::string spassword, IpAddress ip, unsigned short port) {
	if (spassword != Password && Password != "" || db.find(nickname) && db.getPassword(nickname) != password && !db.getPassword(nickname).empty()) {
		Packet sPacket;
		sPacket << (Uint8)'R';
		sPacket << (String)"wrong password";
		send(sPacket, ip, port);
		return false;
	}
	if (db.find(nickname) && db.getBan(nickname)) {
		Packet sPacket;
		sPacket << (Uint8)'R';
		sPacket << (String)"you were banned from the server";
		send(sPacket, ip, port);
		return false;
	}
	int found = -1;
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i].nickname == nickname) {
			found = i;
			break;
		}
	}
	if (found == -1 && newPlayerPos < PLAYERS_COUNT) {
		found = newPlayerPos++;
	}
	if (found != -1) {
		oldConnected = t[found].connected;
		oldIp = t[found].ip;
		oldPort = t[found].port;
		t[found].connect(ip, port, nickname);
		Packet sPacket;
		sPacket << (Uint8)'C';
		send(sPacket, ip, port);
		if (!db.find(nickname)) {
			db.createUser(nickname);
		}
		if (!password.empty()) {
			if (db.getPassword(nickname).empty()) {
				db.setPassword(nickname, password);
			}
			t[found].t.color = db.getColor(nickname);
		}
		for (int i = 0; i < PLAYERS_COUNT; i++) {
			if (clients[i].first.first.getRemoteAddress() == ip && clients[i].first.first.getRemotePort() == port) {
				clients[i].second = true;
			}
		}
		return true;
	}
	return false;
}
void restart(bool changeWL = false, int gamemode = -1, int mapType = randnum(0, 1)) {
	if (gamemode == -1) {
		std::vector<int>weights = { 6, 2, 2 };
		int cur = randnum(0, 9);
		for (gamemode = 0; gamemode < weights.size(); gamemode++) {
			if (cur < weights[gamemode])break;
			cur -= weights[gamemode];
		}
	}
	GAMEMODE = gamemode;
	if (GAMEMODE == 0) {
		MOVEMENT_SPEED = 250;
		ROTATION_SPEED = 270;
		BULLET_SPEED = 400;
		BULLET_LIFETIME = 15;
		BULLET_BOUNCES = -1;
		RELOAD_TIME = 1;
		availablePowerups = { 1, 2, 3, 4, 5, 6 };
		PERMANENT_POWERUP = 0;
	}
	if (GAMEMODE == 1) {
		MOVEMENT_SPEED = 250;
		ROTATION_SPEED = 270;
		BULLET_SPEED = 1600;
		BULLET_LIFETIME = 15;
		BULLET_BOUNCES = 3;
		RELOAD_TIME = 5;
		availablePowerups = { 1, 3, 4, 6 };
		PERMANENT_POWERUP = 0;
	}
	if (GAMEMODE == 2) {
		MOVEMENT_SPEED = 250;
		ROTATION_SPEED = 270;
		BULLET_SPEED = 400;
		BULLET_LIFETIME = 15;
		BULLET_BOUNCES = -1;
		RELOAD_TIME = 5;
		availablePowerups = {};
		PERMANENT_POWERUP = 3;
	}
	mapSeed = rnd();
	b.clear();
	p.clear();
	buildMap(mapType);
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (!t[i].t.dead) {
			if (changeWL) {
				t[i].points++;
				db.setWins(t[i].nickname, db.getWins(t[i].nickname) + 1);
			}
		}
		if (t[i].connected) {
			if (t[i].t.dead && changeWL) {
				db.setLosses(t[i].nickname, db.getLosses(t[i].nickname) + 1);
			}
			t[i].t.dead = false;
		}
		else {
			t[i].t.dead = true;
		}
		t[i].t.bonus = PERMANENT_POWERUP;
		positionTank(t[i].t, Mrandnum(0, mapLength - 1), Mrandnum(0, mapHeight - 1), Mrandnum(0, 3) / (ld)2 * M_PI);
	}
	for (auto& i : t) {
		i.t.lastShot = gettime();
	}
}
bool started = false;
std::deque<std::pair<String, String>>messages;
std::mutex commandMtx;
void handlingmessages() {
	while (true) {
		std::string s;
		std::getline(std::cin, s);
		commandMtx.lock();
		messages.push_back(std::make_pair("", "/" + s));
		commandMtx.unlock();
	}
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
int find(IpAddress ip, unsigned short port) {
	int found = -1;
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i].ip == ip && t[i].port == port) {
			found = i;
			break;
		}
	}
	return found;
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
bool canOperate(std::string nickname) {
	return nickname.empty() || db.getOp(nickname);
}
void broadcast(Packet packet) {
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i].connected) {
			send(packet, t[i].ip, t[i].port);
		}
	}
}
void broadcast(String message) {
	Packet toSend;
	toSend << (Uint8)'B';
	toSend << message;
	broadcast(toSend);
}
std::vector<std::string>buildAutoList() {
	std::vector<std::string>res;
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i].connected) {
			res.push_back(t[i].nickname);
		}
	}
	res.push_back("/start");
	res.push_back("/stop");
	res.push_back("/op");
	res.push_back("/deop");
	res.push_back("/kill");
	res.push_back("/kick");
	res.push_back("/setscore");
	res.push_back("/setbonus");
	res.push_back("/setcolor");
	res.push_back("/changepassword");
	res.push_back("/ban");
	res.push_back("/unban");
	return res;
}
Packet getAutoPacket() {
	Packet res;
	res << (Uint8)'L';
	auto autoList = buildAutoList();
	res << (int)autoList.size();
	for (auto s : autoList) {
		res << s;
	}
	return res;
}
void disconnect(IpAddress ip, unsigned short port) {
	clientsMtx.lock();
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (clients[i].first.first.getRemoteAddress() == ip && clients[i].first.first.getRemotePort() == port) {
			selector.remove(clients[i].first.first);
			clients[i].first.first.disconnect();
			clients[i].first.second = gettime() - DISCONNECT_AFTER - 1;
			clients[i].second = false;
		}
	}
	clientsMtx.unlock();
}
Packet getStartPacket(ld gameTime, int thisTankNum) {
	Packet state;
	state << (Uint8)'0';

	state << GAMEMODE;
	state << MOVEMENT_SPEED;
	state << ROTATION_SPEED;
	state << BULLET_SPEED;
	state << BULLET_LIFETIME;
	state << BULLET_BOUNCES;
	state << RELOAD_TIME;

	state << thisTankNum;
	state << gettime() - gameTime;

	state << mapSeed;
	state << mapLength;
	state << mapHeight;
	state << (int)w.size();
	for (auto i : w.w) {
		state << i.x;
		state << i.y;
		state << i.vertical;
	}

	state << PLAYERS_COUNT;
	for (auto i : t) {
		i.loadToSend(state, gameTime);
	}

	state << (int)b.size();
	for (auto i : b) {
		i.loadToSend(state, gameTime);
	}

	state << (int)lasers.size();
	for (auto i : lasers) {
		i.loadToSend(state);
	}

	state << (int)p.size();
	for (auto i : p) {
		i.loadToSend(state);
	}

	return state;
}
std::vector<player>oldt;
Packet getUpdatePacket(ld gameTime) {
	Packet state;
	state << (Uint8)'U';

	state << gettime() - gameTime;

	oldt.resize(PLAYERS_COUNT);
	int cnt = 0;
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		cnt += t[i] != oldt[i];
	}

	state << cnt;
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i] != oldt[i]) {
			t[i].loadToSend(state, gameTime);
			oldt[i] = t[i];
		}
	}

	state << (int)recentlyChangedWalls.size();
	for (auto i : recentlyChangedWalls) {
		state << i.first;
		i.second.loadToSend(state);
	}

	state << (int)recentlyChangedBullets.size();
	for (auto i : recentlyChangedBullets) {
		state << i.first;
		if (!i.first) {
			int found = -1;
			for (int j = 0; j < b.size(); j++) {
				if (b[j].uid == i.second) {
					found = j;
					break;
				}
			}
			if (found != -1) {
				b[found].loadToSend(state, gameTime);
			}
			else {
				throw(0);
			}
		}
		else {
			state << i.second;
		}
	}

	state << (int)lasers.size();
	for (auto i : lasers) {
		i.loadToSend(state);
	}

	state << (int)recentlyChangedPowerups.size();
	for (auto i : recentlyChangedPowerups) {
		state << i.first;
		if (!i.first) {
			int found = -1;
			for (int j = 0; j < p.size(); j++) {
				if (p[j].uid == i.second) {
					found = j;
					break;
				}
			}
			if (found != -1) {
				p[found].loadToSend(state);
			}
			else {
				throw(0);
			}
		}
		else {
			state << i.second;
		}
	}

	recentlyChangedBullets.clear();
	lasers.clear();
	recentlyChangedPowerups.clear();
	recentlyChangedWalls.clear();

	return state;
}
int main() {
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
	db.load();
	Password = settings["server_password"];
	std::thread pThread(handlingPackets);

	t.resize(PLAYERS_COUNT);
	for (int i = 0; i < PLAYERS_COUNT; i++)t[i].t.id = i;

	ld lastUpdated = gettime();
	ld sendTime = (ld)1 / PACKETS_PER_SECOND;
	ld lastSent = gettime();
	bool waitingForRestart = false;
	ld finishedTime;
	ld lastPowerupSpawned;

	int cntPlayers;

	std::thread cThread(handlingmessages);

	ld gameTime;
	while (running) {
		commandMtx.lock();
		rcMtx.lock();
		if (!rc['/'].empty()) {
			IpPortPacket rPacket = rc['/'].front();
			rc['/'].pop_front();
			rcMtx.unlock();
			int found = find(rPacket.ip, rPacket.port);
			if (found != -1) {
				String temp;
				rPacket.packet >> temp;
				messages.push_back(std::make_pair(t[found].nickname, temp));
			}
		}
		else rcMtx.unlock();
		while (!messages.empty()) {
			auto cur = messages.front();
			messages.pop_front();
			bool executed = false;
			if (cur.second[0] == '/') {
				std::vector<String>v = parseCommand(cur.second.substring(1, (int)cur.second.getSize() - 1));
				if (!v.empty()) {
					for (auto& s : v) {
						if (s == "@s") {
							s = cur.first;
						}
					}
					if (v[0] == "start" && canOperate(cur.first)) {
						waitingForRestart = false;
						executed = true;
						lastPowerupSpawned = gettime() - POWERUP_TIME;
						restart();
						started = true;
						gameTime = gettime();
						cntPlayers = 0;
						for (int i = 0; i < PLAYERS_COUNT; i++) {
							cntPlayers += t[i].connected;
							if (t[i].connected) {
								send(getStartPacket(gameTime, i), t[i].ip, t[i].port);
							}
						}
					}
					if (v[0] == "stop" && canOperate(cur.first)) {
						executed = true;
						running = false;
					}
					if (v[0] == "op" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 2) {
							db.op(v[1]);
						}
					}
					if (v[0] == "deop" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 2) {
							db.deop(v[1]);
						}
					}
					if (v[0] == "kill" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 2) {
							for (int i = 0; i < PLAYERS_COUNT; i++) {
								if (t[i].nickname == v[1] || v[1] == "@a") {
									t[i].t.dead = true;
								}
							}
						}
					}
					if (v[0] == "kick" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 2) {
							for (int i = 0; i < PLAYERS_COUNT; i++) {
								if (t[i].nickname == v[1]) {
									Packet sPacket;
									sPacket << (Uint8)'D';
									sPacket << (String)"kicked from the server";
									send(sPacket, t[i].ip, t[i].port);
									t[i].disconnect();
									disconnect(t[i].ip, t[i].port);
									std::cout << t[i].nickname << " (" << t[i].ip << ":" << t[i].port << ") disconnected: kicked from the server.\n";
									broadcast(t[i].nickname + "was kicked from the server.");
									broadcast(getAutoPacket());
									break;
								}
							}
						}
					}
					if (v[0] == "setscore" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 3 && isNum(v[2])) {
							long long num;
							if (v[2].getSize() >= 12)num = (1ll << 31ll) - 1ll;
							else if (stoll((std::string)v[2]) >= (1ll << 31ll) - 1ll)num = (1ll << 31ll) - 1ll;
							else num = stoi((std::string)v[2]);
							for (int i = 0; i < PLAYERS_COUNT; i++) {
								if (t[i].nickname == v[1] || v[1] == "@a") {
									t[i].points = num;
								}
							}
						}
					}
					if (v[0] == "setbonus" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 3 && isNum(v[2]) && v[2].getSize() <= 4) {
							int num = stoi((std::string)v[2]);
							if (0 <= num && num <= 255) {
								for (int i = 0; i < PLAYERS_COUNT; i++) {
									if (t[i].nickname == v[1] || v[1] == "@a") {
										t[i].t.bonus = num;
										t[i].t.bonusPickedUp = gettime();
									}
								}
							}
						}
					}
					if (v[0] == "setcolor" && canOperate(cur.first)) {
						executed = true;
						if (v.size() == 3) {
							Uint32 color = db.getColor(v[1]);
							if (v[2] == "red") {
								color = Color::Red.toInteger();
							}
							else if (v[2] == "green") {
								color = Color::Green.toInteger();
							}
							else if (v[2] == "blue") {
								color = Color::Blue.toInteger();
							}
							else if (v[2] == "white") {
								color = Color::White.toInteger();
							}
							else if (v[2] == "black") {
								color = Color::Black.toInteger();
							}
							else if (!v[2].isEmpty() && v[2][0] == '#') {
								String c = v[2].substring(1, (int)v[2].getSize() - 1);
								if ((c.getSize() == 6 || c.getSize() == 8) && is16Num(c) && c[0] != '-') {
									size_t sz;
									color = std::stoull((std::string)c, &sz, 16);
									if (c.getSize() == 6)color = color * 256 + 255;
								}
							}
							for (int i = 0; i < PLAYERS_COUNT; i++) {
								if (t[i].nickname == v[1]) {
									db.setColor(v[1], color);
									t[i].t.color = color;
								}
							}
						}
					}
					if (v[0] == "changepassword") {
						executed = true;
						if (v.size() == 3) {
							if (db.getPassword(cur.first) == v[1]) {
								db.setPassword(cur.first, v[2]);
							}
						}
					}
					if (v[0] == "ban") {
						executed = true;
						if (v.size() == 2) {
							db.ban(v[1]);
							for (int i = 0; i < PLAYERS_COUNT; i++) {
								if (t[i].nickname == v[1]) {
									Packet sPacket;
									sPacket << (Uint8)'D';
									sPacket << (String)"banned from the server";
									send(sPacket, t[i].ip, t[i].port);
									t[i].disconnect();
									disconnect(t[i].ip, t[i].port);
									std::cout << t[i].nickname << " (" << t[i].ip << ":" << t[i].port << ") disconnected: banned from the server.\n";
									broadcast(t[i].nickname + "was banned from the server.");
									broadcast(getAutoPacket());
									break;
								}
							}
						}
					}
					if (v[0] == "unban") {
						executed = true;
						if (v.size() == 2) {
							db.unban(v[1]);
						}
					}
					if (!cur.first.isEmpty()) {
						String msg = cur.first + " executed command " + cur.second;
						std::cout << (std::string)msg << '\n';
						Packet toSend;
						toSend << (Uint8)'B';
						toSend << msg;
						for (int i = 0; i < PLAYERS_COUNT; i++) {
							if (t[i].connected && db.getOp(t[i].nickname)) {
								send(toSend, t[i].ip, t[i].port);
							}
						}
					}
				}
				if (!cur.first.isEmpty()) {
					Packet result;
					result << (Uint8)'B';
					if (executed) result << (String)"Command executed successfully!";
					else result << (String)"Couldn't execute the command.";
					for (int i = 0; i < PLAYERS_COUNT; i++) {
						if (t[i].nickname == cur.first) {
							send(result, t[i].ip, t[i].port);
						}
					}
				}
			}
			else {
				String temp = (String)"<" + cur.first + "> " + cur.second;
				std::cout << (std::string)temp << '\n';
				broadcast(temp);
				broadcast(getAutoPacket());
			}
		}
		commandMtx.unlock();
		rcMtx.lock();
		if (!rc['C'].empty()) {
			IpPortPacket rPacket = rc['C'].front();
			rc['C'].pop_front();
			rcMtx.unlock();
			std::string nickname, password, spassword;
			rPacket.packet >> nickname >> password >> spassword;
			if (connect(nickname, password, spassword, rPacket.ip, rPacket.port)) {
				int found = find(rPacket.ip, rPacket.port);
				if (oldConnected) {
					std::cout << t[found].nickname << " (" << oldIp << ":" << oldPort << ") disconnected: disconnected.\n";
					broadcast(t[found].nickname + " left the server.");
					broadcast(getAutoPacket());
					disconnect(oldIp, oldPort);
				}
				t[found].timeSinceLastPacket = gettime();
				std::cout << nickname << " (" << rPacket.ip << ":" << rPacket.port << ") connected!\n";
				broadcast(nickname + " joined the server.");
				broadcast(getAutoPacket());
				if (started)send(getStartPacket(gameTime, found), t[found].ip, t[found].port);
			}
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['P'].empty()) {
			IpPortPacket received = rc['P'].front();
			rc['P'].pop_front();
			rcMtx.unlock();
			int found = find(received.ip, received.port);
			if (found != -1 && t[found].connected) {
				t[found].timeSinceLastPacket = gettime();
				int num;
				received.packet >> num;
				Packet sPacket;
				sPacket << (Uint8)'P' << num;
				send(sPacket, t[found].ip, t[found].port);
			}
		}
		else rcMtx.unlock();
		rcMtx.lock();
		if (!rc['D'].empty()) {
			IpPortPacket received = rc['D'].front();
			rc['D'].pop_front();
			rcMtx.unlock();
			int found = find(received.ip, received.port);
			if (found != -1 && t[found].connected) {
				String reason;
				received.packet >> reason;
				std::cout << t[found].nickname << " (" << t[found].ip << ":" << t[found].port << ") disconnected: " << (std::string)reason << ".\n";
				broadcast(t[found].nickname + " left the server.");
				t[found].disconnect();
				disconnect(t[found].ip, t[found].port);
				broadcast(getAutoPacket());
			}
		}
		else rcMtx.unlock();
		for (int i = 0; i < PLAYERS_COUNT; i++) {
			if (t[i].connected && gettime() - t[i].timeSinceLastPacket >= DISCONNECT_AFTER) {
				std::cout << t[i].nickname << " (" << t[i].ip << ":" << t[i].port << ") disconnected: timed out.\n";
				broadcast(t[i].nickname + " left the server.");
				t[i].disconnect();
				disconnect(t[i].ip, t[i].port);
				broadcast(getAutoPacket());
			}
		}
		if (started) {
			rcMtx.lock();
			if (!rc['S'].empty()) {
				IpPortPacket received = rc['S'].front();
				rc['S'].pop_front();
				rcMtx.unlock();
				int found = find(received.ip, received.port);
				if (found != -1 && !t[found].connected && t[found].ip == received.ip && t[found].port == received.port)t[found].connect(t[found].ip, t[found].port, t[found].nickname);
				if (found != -1 && t[found].connected) {
					t[found].timeSinceLastPacket = gettime();
					int temp;
					received.packet >> temp;
					if (gettime() - gameTime >= 2 && temp == mapSeed) {
						received.packet >> t[found].pressed;
						received.packet >> t[found].t.x;
						received.packet >> t[found].t.y;
						received.packet >> t[found].t.angle;
					}
				}
			}
			else rcMtx.unlock();
			if (gettime() - lastSent > sendTime) {
				lastSent = gettime();
				Packet state = getUpdatePacket(gameTime);
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					if (t[i].connected) {
						send(state, t[i].ip, t[i].port);
					}
				}
			}
			if (gettime() - lastUpdated > UPDATE_TIME) {
				ld dt = gettime() - lastUpdated;
				lastUpdated = gettime();
				int cntAlive = 0, cntConnected = 0;
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					cntAlive += !t[i].t.dead;
					cntConnected += t[i].connected;
				}
				if (cntPlayers > 1 && cntAlive <= 1 || cntPlayers == 1 && (cntAlive == 0 || cntConnected > 1) || cntPlayers == 0 && cntConnected > 0) {
					if (!waitingForRestart) {
						waitingForRestart = true;
						finishedTime = gettime();
					}
				}
				if (waitingForRestart && gettime() - finishedTime >= TIME_BEFORE_RESTART) {
					waitingForRestart = false;
					lastPowerupSpawned = gettime() - POWERUP_TIME;
					restart(cntPlayers > 1);
					gameTime = gettime();
					cntPlayers = 0;
					for (int i = 0; i < PLAYERS_COUNT; i++) {
						cntPlayers += t[i].connected;
						if (t[i].connected) {
							send(getStartPacket(gameTime, i), t[i].ip, t[i].port);
						}
					}
				}
				if (gettime() - gameTime >= 2) {
					for (auto& i : t) {
						i.checkAction(w, b, lasers, dt);
					}
				}
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					if (t[i].t.dead)continue;
					int cur = t[i].t.collided(b);
					if (cur != -1) {
						if (cntPlayers > 1) {
							db.setKills(t[b[cur].id].nickname, db.getKills(t[b[cur].id].nickname) + 1);
							db.setDeaths(t[i].nickname, db.getDeaths(t[i].nickname) + 1);
						}
						destroyBullet(b, w, cur);
						t[i].t.dead = true;
					}
				}
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					if (t[i].t.dead)continue;
					int cur = t[i].t.collided(lasers);
					if (cur != -1) {
						if (cntPlayers > 1) {
							db.setKills(t[lasers[cur].id].nickname, db.getKills(t[lasers[cur].id].nickname) + 1);
							db.setDeaths(t[i].nickname, db.getDeaths(t[i].nickname) + 1);
						}
						t[i].t.dead = true;
					}
				}
				for (int i = 0; i < b.size(); i++) {
					if (b[i].timer + dt > BULLET_LIFETIME || b[i].pos == b[i].path.size()) {
						destroyBullet(b, w, i);
						i--;
					}
					else {
						b[i].move(dt);
					}
				}
				for (int i = 0; i < PLAYERS_COUNT; i++) {
					if (t[i].t.dead)continue;
					int cur = t[i].t.collided(p);
					if (cur != -1 && (!t[i].t.bonus || t[i].t.bonus == 2 && p[cur].type == 2)) {
						t[i].t.bonus = p[cur].type;
						t[i].t.bonusPickedUp = gettime();
						destroyPowerup(p, cur);
					}
					else {
						if (t[i].t.bonus == 2) {
							if (gettime() - t[i].t.bonusPickedUp > INVISIBILITY_TIME) {
								t[i].t.bonus = 0;
							}
						}
					}
				}
				if (gettime() - lastPowerupSpawned > POWERUP_TIME) {
					lastPowerupSpawned = gettime();
					ld curx = getMapX(randnum(0, mapLength - 1)), cury = getMapY(randnum(0, mapHeight - 1));
					bool good = true;
					for (auto i : p) {
						if (curx == i.x && cury == i.y) {
							good = false;
							break;
						}
					}
					if (good && !availablePowerups.empty()) {
						addPowerup(p, powerup(curx, cury, availablePowerups[randnum(1, (int)availablePowerups.size() - 1)]));
					}
				}
			}
		}
	}
	Packet disconnectPacket;
	disconnectPacket << (Uint8)'D';
	disconnectPacket << (String)"server closed";
	for (int i = 0; i < PLAYERS_COUNT; i++) {
		if (t[i].connected)send(disconnectPacket, t[i].ip, t[i].port);
	}
	handling = false;
	pThread.detach();
	cThread.detach();
	return 0;
}