
var Icons = {};
var In = ['g1.png', 'g2.png', 'g3.png', 'g4.png', 'g5.png', 'g6.png', 'g7.png', 'g8.png', 'g9.png',
		'g11.png', 'g12.png', 'g13.png', 'g14.png', 'g15.png', 'g16.png', 'g17.png', 'g18.png', 'g19.png',
		's1.png', 's2.png', 's3.png', 's4.png', 's5.png', 's6.png', 's7.png', 's8.png', 's9.png',
		's10.png', 's11.png', 's12.png', 's13.png', 's14.png', 's15.png', 's16.png', 's17.png', 's18.png',
		's21.png', 's22.png', 's23.png', 's24.png', 's25.png', 's26.png', 's27.png', 's28.png', 's29.png',
		's30.png', 's31.png', 's32.png', 's33.png', 's34.png', 's35.png', 's36.png', 's37.png', 's38.png'
	];
for (var name of In) {Icons [name] = new Image (); Icons [name] . src = name;}

var RandomSystemIcon = function () {
	var s = [
		's1.png', 's2.png', 's3.png', 's4.png', 's5.png', 's6.png', 's7.png', 's8.png', 's9.png',
		's10.png', 's11.png', 's12.png', 's13.png', 's14.png', 's15.png', 's16.png', 's17.png', 's18.png',
		's21.png', 's22.png', 's23.png', 's24.png', 's25.png', 's26.png', 's27.png', 's28.png', 's29.png',
		's30.png', 's31.png', 's32.png', 's33.png', 's34.png', 's35.png', 's36.png', 's37.png', 's38.png'
	];
	return s [Math . floor (Math . random () * s . length)];
};

var RandomWarpIcon = function () {
	var g = [
		'g1.png', 'g2.png', 'g3.png', 'g4.png', 'g5.png', 'g6.png', 'g7.png', 'g8.png', 'g9.png',
		'g11.png', 'g12.png', 'g13.png', 'g14.png', 'g15.png', 'g16.png', 'g17.png', 'g18.png', 'g19.png'
	];
	return g [Math . floor (Math . random () * g . length)];
};

var Ship = function (galaxy, ship) {this . galaxy = galaxy; this . ship = ship; this . source = null; this . SourceName = null;};
Ship . prototype . move = function (target) {
	this . source = null; this . SourceName = null;
	if (this . ship . move < 1) return this;
	if (typeof this . ship . location === 'string') {
		var location = this . galaxy . stars [this . ship . location] . location;
		var ships = this . galaxy . ShipsAt (location . x, location . y);
		for (var ship in ships) {
			if (ships [ship] . race !== this . ship . race) {console . log (`${ships [ship] . race} is present.`); return this;}
		}
	}
	if (typeof target === 'string') {
		for (var ind in this . galaxy . warps) {
			var warp = this . galaxy . warps [ind];
			if ((warp . from === this . ship . location && warp . to === target) ||
				(warp . to === this . ship . location && warp . from === target)) {
				this . ship . location = target; this . ship . move --; return this;
			}
		}
	} else {
		var st = this . ship . location;
		if (typeof st === 'string') st = {x: this . galaxy . stars [st] . location . x, y: this . galaxy . stars [st] . location . y};
		if (target . x === 0 && target . y === 0) return this;
		if (target . x > 1 || target . x < -1 || target . y > 1 || target . y < -1) return this;
		if (target . x + target . y === 0 && target . x !== target . y) return this;
		st . x = pmod (st . x + target . x, this . galaxy . size . width); st . y = pmod (st . y + target . y, this . galaxy . size . height);
		this . ship . location = st;
		this . ship . move --;
		for (var star in this . galaxy . stars) {
			if (this . galaxy . stars [star] . location . x === st . x && this . galaxy . stars [star] . location . y === st . y)
				this . ship . location = star;
		}
		return this;
	}
	return this;
};
Ship . prototype . CanRepair = function () {
	if (this . galaxy . Phase !== 'build') return false;
	var location = this . ship . location;
	if (typeof location !== 'string') return false;
	var race = this . ship . race;
	var Base = this . galaxy . races [race] . bases [location];
	if (Base && Base . construction >= 30) return location;
	for (var ship in this . galaxy . races [race] . ships) {
		var Ship = this . galaxy . races [race] . ships [ship];
		if (Ship . location === location && Ship . Holds > 0) {
			for (var rship in this . galaxy . races [race] . ships) {
				var RShip = this . galaxy . races [race] . ships [rship];
				if (RShip . location === location && RShip . RepairBay) return Ship . name;
			}
		}
	}
	location = this . galaxy . races [race] . ships [location];
	if (location) location = location . location;
	Base = this . galaxy . races [race] . bases [location];
	if (Base && Base . construction >= 30) return location;
	for (var ship in this . galaxy . races [race] . ships) {
		var Ship = this . galaxy . races [race] . ships [ship];
		if (Ship . location === location && Ship . Holds > 0) {
			for (var rship in this . galaxy . races [race] . ships) {
				var RShip = this . galaxy . races [race] . ships [rship];
				if (RShip . location === location && RShip . RepairBay) return Ship . name;
			}
		}
	}
	return false;
};
Ship . prototype . Source = function (ship) {
	this . source = null; this . SourceName = null;
	var Base = this . galaxy . races [this . ship . race] . bases [this . ship . location];
	if (Base && Base . construction >= 30) {this . source = Base; this . SourceName = this . ship . location; return this;}
	var Ship = this . galaxy . races [this . ship . race] . ships [ship];
	if (Ship && this . ship . location === Ship . location && Ship . BuildPoints > 0) {this . source = Ship; this . SourceName = Ship . name; return this;}
	var Carrier = this . galaxy . races [this . ship . race] . ships [this . ship . location];
	if (Ship && Carrier && Carrier . location === Ship . location && Ship . BuildPoints > 0) {this . source = Ship; this . SourceName = Ship . name; return this;}
	return this;
};
Ship . prototype . PowerDrive = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . PowerDrive += pd; this . ship . original . PowerDrive += pd; return this;
};
Ship . prototype . WarpGenerator = function () {
	if (this . source === null || this . source . BuildPoints < 5) return this;
	this . ship . icon = RandomWarpIcon ();
	this . source . BuildPoints -= 5; this . ship . WarpGenerator = true; this . ship . original . WarpGenerator = true; return this;
};
Ship . prototype . Beams = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Beams += pd; this . ship . original . Beams += pd; return this;
};
Ship . prototype . Shields = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Shields += pd; this . ship . original . Shields += pd; return this;
};
Ship . prototype . ECM = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . ECM += pd; this . ship . original . ECM += pd; return this;
};
Ship . prototype . Tubes = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Tubes += pd; this . ship . original . Tubes += pd; return this;
};
Ship . prototype . Missiles = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Missiles += pd * 3; this . ship . original . Missiles += pd * 3; return this;
};
Ship . prototype . Armor = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Armor += pd * 2; this . ship . original . Armor += pd * 2; return this;
};
Ship . prototype . Cannons = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Cannons += pd; this . ship . original . Cannons += pd; return this;
};
Ship . prototype . Shells = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd) return this;
	this . source . BuildPoints -= pd; this . ship . Shells += pd * 6; this . ship . original . Shells += pd * 6; return this;
};
Ship . prototype . Bays = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd || ! this . ship . original . WarpGenerator) return this;
	this . source . BuildPoints -= pd; this . ship . original . Bays += pd;
	for (var ind = 0; ind < pd; ind ++) this . ship . Bays . push (null);
	return this;
};
Ship . prototype . Holds = function (pd) {
	if (this . source === null || this . source . BuildPoints < pd || ! this . ship . original . WarpGenerator) return this;
	this . source . BuildPoints -= pd; this . ship . Holds += pd * 10; this . ship . original . Holds += pd * 10; return this;
};
Ship . prototype . RepairBay = function () {
	if (this . source === null || this . source . BuildPoints < 5 || ! this . ship . original . WarpGenerator) return this;
	this . source . BuildPoints -= 5; this . ship . RepairBay = true; this . ship . original . RepairBay = true; return this;
};
Ship . prototype . Icon = function (icon) {this . ship . icon = icon;};
Ship . prototype . Drop = function (bay) {
	if (typeof this . ship . location !== 'string') return this;
	if (this . galaxy . Phase === 'move' && this . ship . move < 1) return this;
	var ship = this . ship . Bays [bay];
	ship = this . galaxy . races [this . ship . race] . ships [ship];
	ship . location = this . ship . location;
	this . ship . Bays [bay] = null;
	if (this . galaxy . Phase === 'move') this . ship . move --;
	return this;
};
Ship . prototype . Pick = function (ship, bay) {
	if (typeof this . ship . location !== 'string') return this;
	if (this . galaxy . Phase === 'move' && this . ship . move < 1) return this;
	ship = this . galaxy . races [this . ship . race] . ships [ship];
	if (! ship) return this;
	this . ship . Bays [bay] = ship . name;
	ship . location = this . ship . name;
	if (this . galaxy . Phase === 'move') this . ship . move --;
	return this;
};
Ship . prototype . Repair = function (attribute, bp, delta) {
	if (delta === undefined) delta = 1;
	if (! this . source) return this;
	if (this . source . BuildPoints < bp) return this;
	this . ship [attribute] += delta;
	if (this . ship [attribute] > this . ship . original [attribute]) this . ship [attribute] = this . ship . original [attribute];
	this . source . BuildPoints -= bp;
	return this;
};
Ship . prototype . BayRepair = function (ind) {
	if (! this . source) return this;
	if (this . source . BuildPoints < 1) return this;
	if (this . ship . Bays . length <= ind) return this;
	if (this . ship . Bays [ind] !== 'damage') return this;
	this . ship . Bays [ind] = null;
	this . source . BuildPoints --;
	return this;
};

