
var GetName = function (names) {return names . splice (Math . floor (Math . random () * names . length), 1) [0];};
var pmod = function (p, mod) {if (p >= 0) return p % mod; while (p < 0) p += mod; return p;};

var GenerateLocation = function (x, y) {return {x: Math . floor (Math . random () * x), y: Math . floor (Math . random () * y)};};

var Galaxy = function (x, y) {
	this . names = [];
	for (var name of Galaxy . prototype . names) this . names . push (name);
	this . constellations = [];
	for (var name of Galaxy . prototype . constellations) this . constellations . push (name);
	this . size = {width: x, height: y, radius: 32};
	var factor = Math . pow (3, 0.5);
	var left = this . size . height * this . size . radius; var right = this . size . width * factor * this . size . radius;
	var together = left + right;
	var margin = (window . innerWidth - together) * 0.5;
	var height = this . size . radius * this . size . height * 1.5;
	// console . log (left, right, together, window . innerWidth, margin);
	this . shift = {x: margin + left - this . size . radius * 2 + factor * 0.5 * this . size . radius - this . size . radius * 0.5,
		y: (window . innerHeight - height - this . size . radius * 0.5) * 0.5};
	var t = Math . random ();
	var NumberOfStars = Math . floor (x * y * (1.1 * t + 0.9 * (1 - t)) / 12);
	// console . log (NumberOfStars);
	this . stars = {};
	var economies = [0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5];
	for (var ind = 0; ind < NumberOfStars; ind ++) {
		var location = GenerateLocation (x, y);
		while (this . StarAround (location . x, location . y) !== null) location = GenerateLocation (x, y);
		this . stars [GetName (this . names)] = {location: location, economy: economies [Math . floor (Math . random () * economies . length)]};
	}
	this . warps = [];
	for (var star in this . stars) {
		var destinations = []
		for (var ind in this . stars) {
			if (star !== ind) destinations . push ({star: ind, distance: this . distance (star, ind)});
		}
		destinations . sort (function (a, b) {return a . distance < b . distance ? -1 : a . distance > b . distance ? 1 : 0;});
		for (var sub = 0; sub < 4 && sub < destinations . length; sub ++)
			if (Math . random () < 0.25) this . warps . push ({from: star, to: destinations [sub] . star});
	}
	this . races = {};
	this . TechnologyLevel = 0;
	this . TechnologyStep = 6;
	this . Phase = 'build';
	this . Turns = [];
	this . SelectedShip = null;
	this . CombatLocation = null;
	this . CombatShip = null;
	this . Orders = {};
	this . Conflicts = {};
	this . Report = [];
	this . BackgroundColour = '#000033';
	this . NumberOfIneffectiveRounds = 3;
};

Galaxy . prototype . names = ["Acamar", "Achernar", "Achird", "Acrux", "Acubens", "Adara", "Adhafera", "Adhil", "Agena", "Ain al Rami", "Ain", "Al Anz", "Al Kalb al Rai", "Al Minliar al Asad", "Al Minliar al Shuja", "Aladfar", "Alathfar", "Albaldah", "Albali", "Albireo", "Alchiba", "Alcor", "Alcyone", "Aldebaran", "Alderamin", "Aldhibah", "Alfecca Meridiana", "Alfirk", "Agenib", "Aalgiea", "Algol", "Algorab", "Alhena", "Alioth", "Alkaid", "Alkalurops", "Alkes", "Alkurhah", "Almaak", "Alnair", "Alnath", "Alnilam", "Alnitak", "Alniyat", "Alphard", "Alphekka", "Alpheratz", "Alrai", "Alrisha", "Alsafi", "Alsciaukat", "Alshain", "Alshat", "Alsuhail", "Altair", "Altarf", "Alterf", "Aludra", "Alula Australis", "Alula Borealis", "Alya", "Alzirr", "Ancha", "Angetenar", "Ankaa", "Anser", "Antares", "Arcturus", "Arkab Posterior", "Arkab Prior", "Arneb", "Arrakis", "Ascella", "Asellus Australis", "Asellus Borealis", "Asellus Primus", "Asellus Secondus", "Asellus Tertius", "Asterope", "Atik", "Atlas", "Auva", "Avior", "Azelfafage", "Azha", "Azmidiske", "Baham", "Baten Kaitos", "Becrux", "Beid", "Bellatrix", "Betelgeuse", "Botein", "Brachium", "Canopus", "Capella", "Caph", "Castor", "Cebalrai", "Celaeno", "Chara", "Chort", "Cor Caroli", "Cursa", "Dabih", "Deneb Algedi", "Deneb Dulfim", "Deneb el Okab", "Deneb Kaitos Shemali", "Deneb", "Denebola", "Dheneb", "Diadem", "Diphda", "Dschubba", "Dsiban", "Dubhe", "Ed Asich", "Electra", "Elnath", "Enif", "Etamin", "Formalhaut", "Fornacis", "Fum al Samakah", "Furud", "Gacrux", "Gianfar", "Gienah Cygni", "Gienah Ghurab", "Gomeisa", "Gorgonea Quarta", "Gorgonea Secunda", "Gorgonea Tertia", "Graffias", "Grafias", "Grumium", "Hadar", "Haedi", "Hamal", "Hassaleh", "Head of Hydrus", "Heze", "Hoedus II", "Homam", "Hyadum I", "Hyadum II", "Izar", "Jabbah", "Kaffaljidhma", "Kajam", "Kaus Australis", "Kaus Borealis", "Kaus Meridionalis", "Keid", "Kitalpha", "Kocab", "Kornephoros", "Kraz", "Kuma", "Lesath", "Maasym", "Maia", "Marfak", "Marfic", "Marfik", "Markab", "Matar", "Mebsuta", "Megrez", "Meissa", "Mekbuda", "Menkalinan", "Menkar", "Menkent", "Menkib", "Merak", "Merga", "Merope", "Mesarthim", "Metallah", "Miaplacidus", "Minkar", "Mintaka", "Mira", "Mirach", "Miram", "Mirphak", "Mizar", "Mufrid", "Muliphen", "Murzim", "Muscida", "Nair al Saif", "Naos", "Nash", "Nashira", "Nekkar", "Nihal", "Nodus Secundus", "Nunki", "Nusakan", "Peacock", "Phad", "Phaet", "Pherkad Minor", "Pherkad", "Pleione", "Polaris Australis", "Polaris", "Pollux", "Porrima", "Praecipua", "Prima Giedi", "Procyon", "Propus", "Rana", "Ras Elased Australis", "Ras Elased Borealis", "Rasalgethi", "Rasalhague", "Rastaban", "Regulus", "Rigel Kentaurus", "Rigel", "Rijl al Awwa", "Rotanev", "Ruchba", "Ruchbah", "Rukbat", "Sabik", "Sadalachbia", "Sadalmelik", "Sadalsuud", "Sadr", "Saiph", "Salm", "Sargas", "Sarin", "Sceptrum", "Scheat", "Secunda Giedi", "Segin", "Seginus", "Sham", "Sharatan", "Shaula", "Sheidr", "Sheliak", "Sirius", "Situla", "Skat", "Spica", "Sterope II", "Sualocin", "Subra", "Suhail al Muhlif", "Sulafat", "Syrma", "Tabit", "Talitha", "Tania Australis", "Tania Borealis", "Tarazed", "Taygeta", "Tegmen", "Tejat Posterior", "Terebellum", "Thabit", "Theemim", "Thuban", "Torcularis Septentrionalis", "Turais", "Tyl", "Unukalhai", "Vega", "Vindemiatrix", "Wasat", "Wezen", "Wezn", "Yed Posterior", "Yed Prior", "Yildun", "Zaniah", "Zaurak", "Zavijah", "Zibal", "Zosma", "Zuben Elakrab", "Zuben Elakribi", "Zuben Elgenubi", "Zuben Elschemali"];

Galaxy . prototype . constellations = ["Andromeda", "Antlia", "Apus", "Aquarius", "Aquila", "Ara", "Aries", "Auriga", "Boötes", "Caelum", "Camelopardalis", "Cancer", "Canes Venatici", "Canis Major", "Canis Minor", "Capricornus", "Carina", "Cassiopeia", "Centaurus", "Cepheus", "Cetus", "Chamaeleon", "Circinus", "Columba", "Coma Berenices", "Corona Austrina", "Corona Borealis", "Corvus", "Crater", "Crux", "Cygnus", "Delphinus", "Dorado", "Draco", "Equuleus", "Eridanus", "Fornax", "Gemini", "Grus", "Hercules", "Horologium", "Hydra", "Hydrus", "Indus", "Lacerta", "Leo", "Leo Minor", "Lepus", "Libra", "Lupus", "Lynx", "Lyra", "Mensa", "Microscopium", "Monoceros", "Musca", "Norma", "Octans", "Ophiuchus", "Orion", "Pavo", "Pegasus", "Perseus", "Phoenix", "Pictor", "Pisces", "Piscis Austrinus", "Puppis", "Pyxis", "Reticulum", "Sagitta", "Sagittarius", "Scorpius", "Sculptor", "Scutum", "Serpens", "Sextans", "Taurus", "Telescopium", "Triangulum", "Triangulum Australe", "Tucana", "Ursa Major", "Ursa Minor", "Vela", "Virgo", "Volans", "Vulpecula"];

Galaxy . prototype . store = function () {localStorage . setItem ('galaxy', JSON . stringify (this));};
Galaxy . prototype . restore = function () {
	var json = localStorage . getItem ('galaxy');
	if (json === null) return;
	json = JSON . parse (json);
	for (var key in json) this [key] = json [key];
	SelectedShip = null; if (this . SelectedShip) SelectedShip = this . ShipSelect (this . SelectedShip);
};

Galaxy . prototype . CreateRace = function (name, base, colour) {
	if (this . races [name] !== undefined) {alert (`Race ${name} already exists.`); return;}
	for (var race in this . races) {
		// console . log (race, this . races [race]);
		if (this . races [race] . colour === colour) {alert (`Colour ${colour} already taken.`); return;}
		if (this . races [race] . bases [base] !== undefined) {alert (`Star ${base} already taken.`); return;}
	}
	this . races [name] = {colour: colour, bases: {}};
	this . races [name] . bases [base] = {BuildPoints: 20, construction: 30, race: name, colour: colour};
	this . races [name] . ships = {};
	// this . stars [base] . economy = 5;
	this . Turns . push (name);
};

Galaxy . prototype . LoadBuildPoints = function (race, ship, bp) {
	if (this . Phase !== 'build') return;
	ship = this . races [race] . ships [ship];
	var base = this . races [race] . bases [ship . location];
	if (bp > 0 && base && base . BuildPoints >= bp && ship . Holds >= ship . BuildPoints + bp) {base . BuildPoints -= bp; ship . BuildPoints += bp; return;}
	if (bp < 0) {
		bp = - bp;
		if (ship . BuildPoints < bp) return;
		if (base) {
			if (base . construction >= 30) base . BuildPoints += bp; else base . construction += bp;
		} else {
			this . races [race] . bases [ship . location] = {BuildPoints: 0, construction: bp, race: race, colour: ship . colour};
		}
		ship . BuildPoints -= bp;
	}
};

Galaxy . prototype . StarAt = function (x, y) {
	for (star of Object . keys (this . stars)) {
		var location = this . stars [star] . location;
		if (location . x === x && location . y === y) return star;
	}
	return null;
};

Galaxy . prototype . StarAround = function (x, y) {
	var star = this . StarAt (x, y); if (star) return star;
	star = this . StarAt (x, pmod (y + 1, this . size . height)); if (star) return star;
	star = this . StarAt (pmod (x + 1, this . size . width), y); if (star) return star;
	star = this . StarAt (pmod (x + 1, this . size . width), pmod (y + 1, this . size . height)); if (star) return star;
	star = this . StarAt (x, pmod (y - 1, this . size . height)); if (star) return star;
	star = this . StarAt (pmod (x - 1, this . size . width), y); if (star) return star;
	star = this . StarAt (pmod (x - 1, this . size . width), pmod (y - 1, this . size . height)); if (star) return star;
	return null;
};

Galaxy . prototype . RacesAt = function (location) {
	var races = [];
	for (var race in this . races) {
		var Race = this . races [race];
		for (var ship in Race . ships) {
			var Ship = Race . ships [ship];
			if (Ship . location === location && races . indexOf (Ship . race) < 0) races . push (Ship . race);
		}
	}
	return races;
};

Galaxy . prototype . CombatAt = function (location) {return this . RacesAt (location) . length > 1;};

Galaxy . prototype . Combats = function () {
	var locations = [];
	for (var star in this . stars) {
		if (this . CombatAt (star)) locations . push (star);
	}
	return locations;
};

Galaxy . prototype . ResetOrders = function () {
	var combats = this . Combats ();
	// console . log (combats);
	for (var location of combats) {
		// console . log (location);
		for (var race in this . races) {
			var Race = this . races [race];
			for (var ship in Race . ships) {
				var Ship = Race . ships [ship];
				if (Ship . location === location) {
					this . Orders [ship] = {
						race: race, ship: ship, Strategy: 'DODGE', PowerDrive: 0, Target: null,
						Beams: 0, Shields: 0, Tubes: [], Bays: [], ECM: 0, Cannons: [],
						Damage: 0, CanEscape: true, TechnologyLevel: Ship . TechnologyLevel, location: location
					};
					for (var tube = 0; tube < Ship . Tubes; tube ++) this . Orders [ship] . Tubes . push ({Target: null, PowerDrive: 0});
					for (var cannon = 0; cannon < Ship . Cannons; cannon ++) this . Orders [ship] . Cannons . push ({Target: null, Shells: 1});
					for (var bay = 0; bay < Ship . Bays . length; bay ++) this . Orders [ship] . Bays . push (null);
				}
			}
		}
	}
};

var distance = function (dx) {
	if (dx . x >= 0 && dx . y >= 0) return Math . max (dx . x, dx . y);
	if (dx . x <= 0 && dx . y <= 0) return Math . max (- dx . x, - dx . y);
	return Math . abs (dx . x) + Math . abs (dx . y);
};

Galaxy . prototype . distance = function (from, to) {
	if (typeof from === 'string') from = {x: this . stars [from] . location . x, y: this . stars [from] . location . y};
	if (typeof to === 'string') to = {x: this . stars [to] . location . x, y: this . stars [to] . location . y};
	var dx = {x: to . x - from . x, y: to . y - from . y};
	var d1 = distance (dx);
	var d2 = distance ({x: dx . x - this . size . width, y: dx . y});
	var d3 = distance ({x: dx . x + this . size . width, y: dx . y});
	var d4 = distance ({x: dx . x, y: dx . y - this . size . height});
	var d5 = distance ({x: dx . x, y: dx . y + this . size . height});
	var d6 = distance ({x: dx . x - this . size . width, y: dx . y - this . size . height});
	var d7 = distance ({x: dx . x - this . size . width, y: dx . y + this . size . height});
	var d8 = distance ({x: dx . x + this . size . width, y: dx . y - this . size . height});
	var d9 = distance ({x: dx . x + this . size . width, y: dx . y + this . size . height});
	return Math . min (d1, d2, d3, d4, d5, d6, d7, d8, d9);
};

Galaxy . prototype . RawDistance = function (from, to) {
	var dx = {x: to . x - from . x, y: to . y - from . y};
	var d1 = distance (dx);
	var d2 = distance ({x: dx . x - this . size . width, y: dx . y});
	var d3 = distance ({x: dx . x + this . size . width, y: dx . y});
	var d4 = distance ({x: dx . x, y: dx . y - this . size . height});
	var d5 = distance ({x: dx . x, y: dx . y + this . size . height});
	var d6 = distance ({x: dx . x - this . size . width, y: dx . y - this . size . height});
	var d7 = distance ({x: dx . x - this . size . width, y: dx . y + this . size . height});
	var d8 = distance ({x: dx . x + this . size . width, y: dx . y - this . size . height});
	var d9 = distance ({x: dx . x + this . size . width, y: dx . y + this . size . height});
	return Math . min (d1, d2, d3, d4, d5, d6, d7, d8, d9);
};

var GetName = function (names) {return names . splice (Math . floor (Math . random () * names . length), 1) [0];};

Galaxy . prototype . Collect = function () {
	if (this . Phase === 'build') return; this . Phase = 'build';
	this . Turns . shift (galaxy . races);
	if (this . Turns . length < 1) {
		var races = Object . keys (this . races);
		while (races . length > 0) this . Turns . push (races . splice (Math . floor (Math . random () * races . length), 1) [0]);
		this . TechnologyLevel ++;
	}
	if (this . TechnologyLevel < 1) return;
	for (var star in this . stars) {
		var Star = this . stars [star];
		var Base = this . BaseAt (Star . location . x, Star . location . y);
		if (Base && Base . race === this . Turns [0] && Base . construction >= 30) Base . BuildPoints += Star . economy * 2;
		else {
			var Ships = this . ShipsAt (Star . location . x, Star . location . y);
			var BuildPoints = Star . economy;
			for (var ind = 0; BuildPoints > 0 && ind < Ships . length; ind ++) {
				var Ship = Ships [ind];
				if (Ship . race === this . Turns [0]) {
					var capacity = Ship . Holds - Ship . BuildPoints;
					var amount = BuildPoints > capacity ? capacity : BuildPoints;
					BuildPoints -= amount;
					Ship . BuildPoints += amount;
					// console . log (Ship . name, Ship . Holds, Ship . BuildPoints);
				}
			}
		}
	}
};

Galaxy . prototype . Combat = function (CombatLocations) {
	for (var race in this . races) {for (var ship in this . races [race] . ships) {this . races [race] . ships [ship] . move = 0;}}
	this . Conflicts = {};
	if (CombatLocations . length < 1) return;
	if (this . Phase !== 'move') return; this . Phase = 'combat';
	this . CombatLocation = CombatLocations [0];
	for (var combat in CombatLocations) {
		var races = this . RacesAt (CombatLocations [combat]); races . push ('process');
		this . Conflicts [CombatLocations [combat]] = {races: races, ineffective: 0};
		this . CombatShip = null;
	}
	this . ResetOrders ();
};

Galaxy . prototype . ShipSelect = function (name) {
	for (var race in this . races) {
		for (var ship in this . races [race] . ships) {
			if (ship === name) {this . SelectedShip = name; return new Ship (this, this . races [race] . ships [ship]);}
		}
	}
	this . SelectedShip = null;
	return null;
};

Galaxy . prototype . ReMoveShips = function () {
	if (this . Phase === 'move') return; this . Phase = 'move';
	if (this . Turns . length < 1) return this;
	for (var ship in this . races [this . Turns [0]] . ships) {
		var Ship = this . races [this . Turns [0]] . ships [ship];
		if (Ship . WarpGenerator) Ship . move = Ship . PowerDrive;
	}
};

Galaxy . prototype . Next = function () {
	if (this . Turns . length < 1) return;
	switch (this . Phase) {
	case 'build': this . ReMoveShips (); break;
	case 'move':
		var combats = this . Combats ();
		if (combats . length > 0) this . Combat (combats);
		else this . Collect ();
		break;
	case 'combat':
		if (this . CombatLocation && this . RacesAt (this . CombatLocation) . length < 2) {if (this . Combats () . length < 1) this . Collect (); this . CombatLocation = null; break;}
		var conflict = this . Conflicts [this . CombatLocation];
		if (! conflict) {if (this . Combats () . length < 1) this . Collect (); break;}
		if (conflict . races . length < 2) {
			if (this . Combats () . length < 1) {this . Collect (); break;}
			var races = this . RacesAt (this . CombatLocation); races . push ('process');
			if (races . length > 1) conflict . races = races;
			break;
		}
		if (conflict . races . length > 1) {conflict . races . shift (); this . CombatShip = null;}
		if (conflict . races . length < 2) {
			this . Report = [];
			this . ProcessOrders ();
			this . DropShips ();
			this . ApplyDamages ();
			this . PickUpShips ();
			this . RetreatShips ();
			this . ResetOrders ();
			this . ProcessIneffectiveRounds ();
			this . PotentiallyDestroyBase ();
		}
		break;
	case 'rearrange': this . Collect (); break;
	default: break;
	}
};

Galaxy . prototype . ColourForStar = function (star) {
	for (var race in this . races) {
		var Race = this . races [race];
		for (var base in Race . bases) {
			if (base === star) return Race . colour;
		}
	}
	return this . BackgroundColour;
};

Galaxy . prototype . draw = function () {
	var radius = this . size . radius;
	var size = radius + radius;
	var hh = Math . pow (3, 0.5) * radius / 2;
	var hhh = hh + hh;
	var ll = radius * 1.5;
	var l = radius * 0.5;
	var hhx = hh * 1;
	ctx . strokeStyle = 'gray';
	// for (var sub = - Math . floor (this . size . height * 0.5); sub < Math . floor (this . size . height * 1.5); sub ++) {
	for (var sub = 0; sub < this . size . height; sub ++) {
		// for (var ind = - Math . floor (this . size . width * 0.5); ind < Math . floor (this . size . width * 1.5); ind ++) {
		for (var ind = 0; ind < this . size . width; ind ++) {
			ctx . beginPath ();
			ctx . moveTo (this . shift . x - sub * hh + radius + ind * hhh + hhx * 0.9, this . shift . y + sub * ll + radius - l);
			ctx . lineTo (this . shift . x - sub * hh + radius + ind * hhh + hhx * 0.9, this . shift . y + sub * ll + radius + l);
			ctx . lineTo (this . shift . x - sub * hh + radius + ind * hhh, this . shift . y + sub * ll + radius + radius * 0.9);
			ctx . lineTo (this . shift . x - sub * hh + radius + ind * hhh - hhx * 0.9, this . shift . y + sub * ll + radius + l);
			ctx . lineTo (this . shift . x - sub * hh + radius + ind * hhh - hhx * 0.9, this . shift . y + sub * ll + radius - l);
			ctx . lineTo (this . shift . x - sub * hh + radius + ind * hhh, this . shift . y + sub * ll + radius * 0.1);
			ctx . closePath ();
			// ctx . arc (this . shift . x - sub * hh + radius + ind * hhh, this . shift . y + sub * ll + radius, radius, 0, Math . PI * 2);
			ctx . stroke ();
			var Base = this . BaseOrShipAt (ind, sub);
			if (Base !== null) {
				if (Base . location === undefined || typeof Base . location === 'string') {ctx . fillStyle = Base . colour; ctx . fill ();}
				ctx . fillStyle = 'white'; ctx . textAlign = 'center';
				if (Base . BuildPoints !== undefined && Base . name === undefined)
					ctx . fillText (Base . construction >= 30 ? `[${Base . BuildPoints}]` : `<${Base . construction}>`,
						this . shift . x + ind * hhh - sub * hh + radius,
						this . shift . y + sub * ll + radius - 12);
			}
		}
	}
	ctx . strokeStyle = 'blue';
	for (var ind in this . warps) {
		var warp = this . warps [ind];
		var from = this . stars [warp . from] . location; var to = this . stars [warp . to] . location; var toa = [];
		toa . push (to);
		toa . push ({x: to . x - this . size . width, y: to . y});
		toa . push ({x: to . x - this . size . width, y: to . y - this . size . height});
		toa . push ({x: to . x, y: to . y - this . size . height});
		toa . push ({x: to . x + this . size . width, y: to . y - this . size . height});
		toa . push ({x: to . x + this . size . width, y: to . y});
		toa . push ({x: to . x + this . size . width, y: to . y + this . size . height});
		toa . push ({x: to . x, y: to . y + this . size . height});
		toa . push ({x: to . x - this . size . width, y: to . y + this . size . height});
		for (var t in toa) toa [t] . distance = distance ({x: toa [t] . x - from . x, y: toa [t] . y - from . y});
		toa . sort (function (a, b) {return a . distance < b . distance ? -1 : a . distance > b . distance ? 1 : 0;});
		to = toa [0];
		// console . log (warp, from, toa);
		ctx . beginPath ();
		ctx . moveTo (this . shift . x + from . x * hhh - from . y * hh + radius, this . shift . y + from . y * ll + radius);
		ctx . lineTo (this . shift . x + to . x * hhh - to . y * hh + radius, this . shift . y + to . y * ll + radius);
		ctx . stroke ();
		// console . log (from, to);
	}
	for (var race in this . races) {
		for (var ship in this . races [race] . ships) {
			var s = this . races [race] . ships [ship];
			var st = s . location;
			if (typeof st === 'string') {st = this . stars [st]; if (st) st = st . location;}
			// var st = this . stars [s . location] . location;
			if (st) {
				ctx . save ();
				ctx . translate (Icons [s . icon] . width * -0.25, Icons [s . icon] . height * -0.25);
				ctx . translate (this . shift . x, this . shift . y);
				ctx . translate (hh, radius);
				ctx . translate (st . x * hhh - st . y * hh, st . y * ll);
				ctx . scale (0.5, 0.5);
				ctx . drawImage (Icons [s . icon], 0, 0);
				ctx . restore ();
			}
		}
	}
	ctx . textAlign = 'center';
	ctx . fillStyle = 'white';
	for (var star in this . stars) {
		var st = this . stars [star];
		var dx = this . shift . x + st . location . x * hhh - st . location . y * hh + radius;
		var dy = this . shift . y + st . location . y * ll + radius;
		var W = ctx . measureText (star) . width * 0.5;
		ctx . beginPath ();
		ctx . moveTo (dx - W, dy - 10); ctx . lineTo (dx + W, dy - 10); ctx . lineTo (dx + W, dy + 2); ctx . lineTo (dx - W, dy + 2);
		ctx . fillStyle = this . ColourForStar (star); ctx . fill ();
		ctx . fillStyle = 'white';
		ctx . fillText (star, dx, dy);
		ctx . fillText (star, dx - this . size . width * hhh, dy);
		ctx . fillText (star, dx + this . size . width * hhh, dy);
		ctx . fillText (star, dx - this . size . height * hh, dy + this . size . height * ll);
		ctx . fillText (star, dx + this . size . height * hh, dy - this . size . height * ll);
		ctx . fillText (star, dx - this . size . width * hhh - this . size . height * hh, dy + this . size . height * ll);
		ctx . fillText (star, dx + this . size . width * hhh - this . size . height * hh, dy + this . size . height * ll);
		ctx . fillText (star, dx - this . size . width * hhh + this . size . height * hh, dy - this . size . height * ll);
		ctx . fillText (star, dx + this . size . width * hhh + this . size . height * hh, dy - this . size . height * ll);
		ctx . fillText (st . economy, dx, dy + 12);
	}
	for (var star in this . stars) {
		var location = this . stars [star] . location;
		var ships = this . ShipsAt (location . x, location . y);
		var races = [];
		for (var ind in ships) if (races . indexOf (ships [ind] . race) < 0) races . push (ships [ind] . race);
		if (races . length > 1) {
			ctx . strokeStyle = 'red';
			ctx . beginPath ();
			ctx . arc (
				this . shift . x + this . size . radius + location . x * hhh - location . y * hh,
				this . shift . y + this . size . radius + location . y * ll,
				this . size . radius * 1.5, 0, Math . PI * 2);
			ctx . stroke ();
		}
	}
	for (var race in this . races) {
		var Race = this . races [race];
		for (var ship in Race . ships) {
			if (ship === this . SelectedShip) {
				ctx . strokeStyle = 'blue';
				var location = Race . ships [ship] . location;
				if (typeof location === 'string') {
					location = this . stars [location];
					if (location) location = location . location;
					else {
						location = Race . ships [Race . ships [ship] . location] . location;
						if (typeof location === 'string') location = this . stars [location] . location;
					}
				}
				ctx . strokeStyle = 'blue';
				ctx . beginPath ();
				ctx . arc (
					this . shift . x + this . size . radius + location . x * hhh - location . y * hh,
					this . shift . y + this . size . radius + location . y * ll,
					this . size . radius * 1.25, 0, Math . PI * 2);
				ctx . stroke ();
			}
		}
	}
	if (this . CombatLocation && this . Phase === 'combat') {
		ctx . save ();
		ctx . translate (canvas . width * 0.5, canvas . height * 0.5);
		ctx . beginPath ();
		ctx . arc (0, 0, 256, 0, Math . PI * 2);
		ctx . stroke ();
		ctx . fillStyle = 'gold';
		ctx . fill ();
		ctx . fillStyle = 'blue';
		ctx . font = 'bold 48px arial';
		ctx . textBaseline = 'middle';
		ctx . fillText (this . CombatLocation . toUpperCase (), 0, 0);
		var races = this . RacesAt (this . CombatLocation);
		// console . log ('RACES', this . RacesAt (this . CombatLocation));
		var angle = Math . PI * 1.5;
		ctx . font = '18px arial';
		ctx . textBaseline = 'top';
		var sectors = this . ShipsAt (this . stars [this . CombatLocation] . location . x, this . stars [this . CombatLocation] . location . y) . length;
		for (var ind = 0; ind < races . length; ind ++) {
			var Race = this . races [races [ind]];
			for (var ship in Race . ships) {
				var Ship = Race . ships [ship];
				if (Ship . location === this . CombatLocation) {
					var IC = Icons [Ship . icon];
					ctx . save ();
					ctx . scale (0.75, 0.75);
					ctx . translate (IC . width * -0.5, IC . height * -0.5);
					// var angle = Math . random () * Math . PI * 2;
					var dist = Math . random () * 256;
					dist = 256;
					var y = Math . sin (angle) * dist;
					var x = Math . cos (angle) * dist;
					ctx . drawImage (IC, x, y);
					ctx . fillStyle = Ship . colour;
					ctx . fillText (Ship . name, x + IC . width * 0.5, y + IC . height);
					angle += Math . PI * 2 / sectors;
					// console . log (races [ind], ship);
					ctx . restore ();
				}
			}
		}
		ctx . restore ();
	}
};

Galaxy . prototype . RaceFromBase = function (base) {
	for (var race in this . races) {
		for (var b in this . races [race] . bases) {
			if (b === base) return race;
		}
	}
	return null;
};

Galaxy . prototype . BaseAt = function (x, y) {
	for (var race in this . races) {
		for (var base in this . races [race] . bases) {
			var star = this . stars [base];
			if (star . location . x === x && star . location . y === y) return this . races [race] . bases [base];
		}
	}
	return null;
};

Galaxy . prototype . ShipAt = function (x, y) {
	for (var race in this . races) {
		var r = this . races [race];
		for (var ship in r . ships) {
			var s = r . ships [ship];
			var st = s . location;
			if (typeof st === 'string') {st = this . stars [st]; if (st) st = st . location;}
			if (st && st . x === x && st . y === y) return s;
		}
	}
	return null;
};

Galaxy . prototype . ShipsAt = function (x, y) {
	var ships = [];
	for (var race in this . races) {
		var r = this . races [race];
		for (var ship in r . ships) {
			var s = r . ships [ship];
			var st = s . location;
			if (typeof st === 'string') {st = this . stars [st]; if (st) st = st . location;}
			if (st && st . x === x && st . y === y) ships . push (s);
		}
	}
	return ships;
};

Galaxy . prototype . Ship = function (name) {
	for (var race in galaxy . races) {var Race = galaxy . races [race]; for (var ship in Race . ships) if (ship === name) return Race . ships [ship];}
};

Galaxy . prototype . BaseOrShipAt = function (x, y) {return this . BaseAt (x, y) || this . ShipAt (x, y);};

Galaxy . prototype . CreateShip = function (base, name) {
	if (name === undefined) name = GetName (this . constellations);
	var race = this . RaceFromBase (base);
	if (race === null) {alert (`Can not identify race at ${base}.`); return null;}
	if (race !== this . Turns [0]) {alert (`It is now ${this . Turns [0]} turn. Not ${race}.`); return null;}
	if (this . races [race] . bases [base] . construction < 30) {alert (`Base at ${base} incomplete.`); return null;}
	var ship = {
		PowerDrive: 0, WarpGenerator: false, Beams: 0, Shields: 0, ECM: 0, Tubes: 0, Missiles: 0,
		Armor: 0, Cannons: 0, Shells: 0, Bays: [], Holds: 0, RepairBay: false,
		original: {
			PowerDrive: 0, WarpGenerator: false, Beams: 0, Shields: 0, ECM: 0, Tubes: 0, Missiles: 0,
			Armor: 0, Cannons: 0, Shells: 0, Bays: 0, Holds: 0, RepairBay: false},
		location: base, move: 0, name: name, icon: RandomSystemIcon (), TechnologyLevel: this . TechnologyLevel,
		race: race, colour: this . races [race] . colour, BuildPoints: 0};
	this . races [race] . ships [name] = ship;
	return new Ship (this, ship) . Source (base);
};

var CombatResultTable = function (AttackStrategy, AttackPowerDrive, TargetStrategy, TargetPowerDrive, hits) {
	var delta = AttackPowerDrive - TargetPowerDrive;
	switch (AttackStrategy) {
	case 'ATTACK':
		switch (TargetStrategy) {
		case 'ATTACK': switch (delta) {case -2: case -1: return hits; break; case 0: case 1: return hits + 2; break; case 2: return hits + 1; break; default: return 0; break;} break;
		case 'DODGE': switch (delta) {case 2: return hits + 1; break; case 3: case 4: return hits; break; default: return 0; break;} break;
		case 'RETREAT': if (delta < 0) return null; if (delta === 3 || delta === 4) return hits; return 0; break;
		default: return null; break;
		} break;
	case 'DODGE':
		switch (TargetStrategy) {
		case 'ATTACK': if (delta < -1 || delta < 2) return 0; return hits; break;
		case 'DODGE': if (delta < -3 || delta > 0) return 0; return hits; break;
		case 'RETREAT': return null; break;
		default: return null; break;
		} break;
	case 'RETREAT':
		switch (TargetStrategy) {
		case 'ATTACK': if (delta >= -1 && delta <= 0) return hits; return 0; break;
		case 'DODGE': return 0; break;
		case 'RETREAT': return null; break;
		default: return null; break;
		} break;
	default: break;
	}
}

Galaxy . prototype . Damage = function (damage, AttackerTL, TargetTL) {
	var tld = (AttackerTL - TargetTL) / this . TechnologyStep;
	tld = tld >= 0 ? Math . floor (tld) : Math . ceil (tld);
	if (damage > 0) damage = damage + tld;
	if (damage < 0) damage = 0;
	return damage;
};

Galaxy . prototype . ProcessOrder = function (Ship, Order) {
	if (Order . Target) {
		var TargetOrder = this . Orders [Order . Target];
		var damage = CombatResultTable (Order . Strategy, Order . PowerDrive, TargetOrder . Strategy, TargetOrder . PowerDrive, Order . Beams);
		if (damage !== null) {TargetOrder . Damage += this . Damage (damage, TargetOrder . TechnologyLevel, Order . TechnologyLevel); TargetOrder . CanEscape = false;}
		this . Report . push ({text: `${Ship . name} [${Order . PowerDrive}/${Order . Strategy . toLowerCase ()}] beam fire ${Order . Beams} at ${Order . Target} [${this . Orders [Order . Target] . PowerDrive}/${this . Orders [Order . Target] . Strategy . toLowerCase ()}] => inflicts ${damage} damage.`, colour: galaxy . races [Order . race] . colour});
	}
	for (var ind = 0; ind < Order . Tubes . length; ind ++) {
		var Tube = Order . Tubes [ind];
		if (Tube . Target) {
			var TargetOrder = this . Orders [Tube . Target];
			var ECM = TargetOrder . ECM + Math . floor ((Order . TechnologyLevel - TargetOrder . TechnologyLevel) / this . TechnologyStep);
			if (ECM < 0) ECM = 0;
			// console . log (Order . TechnologyLevel, TargetOrder . TechnologyLevel, ECM, TargetOrder);
			var damage1 = CombatResultTable ('ATTACK', Tube . PowerDrive + ECM, TargetOrder . Strategy, TargetOrder . PowerDrive, 2);
			var damage2 = CombatResultTable ('ATTACK', Tube . PowerDrive - ECM, TargetOrder . Strategy, TargetOrder . PowerDrive, 2);
			var damage = null;
			if (damage1 !== null && damage2 !== null) damage = Math . min (damage1, damage2);
			else if (damage1 !== null) damage = damage1; else damage = damage2;
			if (damage !== null) {
				TargetOrder . Damage += this . Damage (damage, TargetOrder . TechnologyLevel, Order . TechnologyLevel);
				// TargetOrder . CanEscape = false;
			}
			Ship . Missiles --;
			// console . log (TargetOrder . Damage, damage, damage1, damage2);
		}
	}
	for (var ind = 0; ind < Order . Cannons . length; ind ++) {
		var Cannon = Order . Cannons [ind];
		if (Cannon . Target) {
			var TargetOrder = this . Orders [Cannon . Target];
			var damage = CombatResultTable (Order . Strategy, Order . PowerDrive, TargetOrder . Strategy, TargetOrder . PowerDrive, Cannon . Shells);
			if (damage !== null) {TargetOrder . Damage += this . Damage (damage, TargetOrder . TechnologyLevel, Order . TechnologyLevel); TargetOrder . CanEscape = false;}
			Ship . Shells -= Cannon . Shells;
			this . Report . push ({text: `${Ship . name} [${Order . PowerDrive} / ${Order . Strategy}] cannon fire ${Cannon . Shells} at ${Cannon . Target} [${this . Orders [Cannon . Target] . PowerDrive} / ${this . Orders [Cannon . Target] . Strategy}] => inflicts ${damage} damage.`, colour: galaxy . races [Order . race] . colour});
		}
	}
};

Galaxy . prototype . ProcessOrdersFor = function (star) {
	for (var ship in this . Orders) {
		var Ship = this . Ship (ship);
		if (Ship . location === star) this . ProcessOrder (Ship, this . Orders [ship]);
	}
};

// Galaxy . prototype . ProcessOrders = function () {for (var ship in this . Orders) this . ProcessOrder (this . Ship (ship), this . Orders [ship]);};
Galaxy . prototype . ProcessOrders = function () {
	for (var ship in this . Orders) {
		if (this . Orders [ship] . location === this . CombatLocation) {
			// console . log (this . Orders [ship]);
			this . ProcessOrder (this . Ship (ship), this . Orders [ship]);
		}
	}
};

Galaxy . prototype . DropShips = function () {
	for (var order in this . Orders) {
		var Order = this . Orders [order];
		if (Order . location === this . CombatLocation) {
			for (var ind = 0; ind < Order . Bays . length; ind ++) {
				var Bay = Order . Bays [ind];
				if (Bay === 'drop') {
					var Carrier = this . Ship (order);
					var SS = this . Ship (Carrier . Bays [ind]);
					SS . location = Carrier . location;
					Carrier . Bays [ind] = null;
				}
			}
		}
	}
};

Galaxy . prototype . ApplyDamages = function () {
	for (var order in this . Orders) {
		var Order = this . Orders [order];
		if (Order . location === this . CombatLocation) {
			if (Order . Damage > 0) {
				// console . log (order, 'DAMAGE', Order . Damage, Order . Shields, Order);
				var damage = Order . Damage - Order . Shields;
				// console . log (`${Order . race} ${Order . ship} takes ${Order . Damage} - ${Order . Shields} = ${damage}.`);
				var SP = new Ship (this, this . races [Order . race] . ships [Order . ship]);
				SP . ApplyDamage (damage);
				// console . log ('ORDER', Order);
				this . Report . push ({text: `${damage} to ${SP . ship . name}.`, colour: SP . ship . colour});
				console . log ('Reset ineffective', this . CombatLocation);
				this . Conflicts [this . CombatLocation] . ineffective = 0;
				// this . stars [Order . location] . ineffective = 0;
				// this . stars [this . Ship (Order . ship) . location] . ineffective = 0;
				// console . log ('DAMAGE', this . stars [this . Ship (Order . ship) . location], this . stars [this . Ship (Order . ship) . location] . ineffective);
			}
		}
	}
};

Galaxy . prototype . PickUpShips = function () {
	for (var order in this . Orders) {
		var Order = this . Orders [order];
		if (Order . location === this . CombatLocation) {
			var Ship = this . Ship (order);
			for (var ind = 0; ind < Order . Bays . length; ind ++) {
				if (Order . Bays [ind] !== null && Order . Bays [ind] !== 'drop' && Ship . Bays [ind] === null) {
					var ss = this . Ship (Order . Bays [ind]);
					if (ss . location === Ship . location) {
						Ship . Bays [ind] = Order . Bays [ind];
						ss . location = Ship . name;
					}
				}
			}
		}
	}
};

Galaxy . prototype . RetreatShips = function () {
	for (var order in this . Orders) {
		var Order = this . Orders [order];
		if (Order . location === this . CombatLocation) {
			if (Order . Strategy === 'RETREAT' && Order . CanEscape) {
				var Ship = this . Ship (order);
				var Location = Ship . location;
				if (typeof Location === 'string') {
					Location = this . stars [Location] . location;
					var shift = [{x: 1, y: 0}, {x: 0, y: 1}, {x: -1, y: 0}, {x: 0, y: -1}, {x: 1, y: 1}, {x: -1, y: -1}];
					shift = shift [Math . floor (Math . random () * shift . length)];
					Ship . location = {x: pmod (Location . x + shift . x, this . size . width), y: pmod (Location . y + shift . y, this . size . height)};
					for (var star in this . stars) {
						if (this . stars [star] . location . x === Ship . location . x && this . stars [star] . location . y === Ship . location . y) Ship . location = star;
					}
					this . CombatShip = null;
					this . Report . push ({text: `${Ship . name} retreated.`, colour: Ship . colour});
				}
			}
		}
	}
};

Galaxy . prototype . DeleteShip = function (race, ship) {
	// console . log (race, ship, this . races [race] . ships);
	var Bays = this . races [race] . ships [ship] . Bays;
	for (var ind = 0; ind < Bays . length; ind ++) if (Bays [ind] && Bays [ind] !== 'damage') this . DeleteShip (race, Bays [ind]);
	if (this . SelectedShip === ship) this . SelectedShip = null;
	if (this . CombatShip === ship) this . CombatShip = null;
	if (SelectedShip && SelectedShip . ship === this . races [race] . ships [ship]) SelectedShip = null;
	delete this . races [race] . ships [ship];
	// console . log (SelectedShip . ship, this . races [race] . ships [ship], SelectedShip . ship === this . races [race] . ships [ship], this . SelectedShip, SelectedShip);
};

Galaxy . prototype . ProcessIneffectiveRounds = function () {if (this . CombatLocation) if (this . Conflicts [this . CombatLocation] . ineffective ++ >= this . NumberOfIneffectiveRounds) this . Exodus ();};

Galaxy . prototype . PotentiallyDestroyBase = function () {
	var races = this . RacesAt (this . CombatLocation);
	if (races && races . length === 1) {
		for (var race in this . races) {
			for (var base in this . races [race] . bases) {
				// console . log (races, race, base, this . races [race] . bases [base]);
				if (races [0] !== race && base === this . CombatLocation) {
					this . Report . push ({text: `${base} base destroyed.`, colour: this . races [race] . colour});
					delete this . races [race] . bases [base];
				}
			}
		}
		this . Report . push ({text: `Combat at ${this . CombatLocation} completed. ${races [0]} is the winner.`, colour: 'white'});
		if (races [0] === this . Turns [0]) this . Report . push ({text: `${this . Turns [0]} can now rearrange ships at ${this . CombatLocation}.`, colour: this . races [this . Turns [0]] . colour});
	}
};

Galaxy . prototype . Exodus = function () {
	// console . log (`Exodus at ${this . CombatLocation}.`);
	for (var ship in this . races [this . Turns [0]]. ships) {
		var Ship = this . races [this . Turns [0]] . ships [ship];
		if (Ship . location === this . CombatLocation) {
			if (! Ship . WarpGenerator) {
				for (var carrier in this . races [this . Turns [0]] . ships) {
					if (Ship . location === this . CombatLocation) {
						var Carrier = this . races [this . Turns [0]] . ships [carrier];
						if (Carrier . WarpGenerator && Carrier . location === this . CombatLocation) {
							for (var ind in Carrier . Bays) {
								if (Ship . location === this . CombatLocation) {
									this . Report . push ({text: `${Ship . name} picked up by ${Carrier . name}.`, colour: Ship . colour});
									Carrier . Bays [ind] = Ship . name;
									Ship . location = Carrier . name;
								}
							}
						}
					}
				}
				if (Ship . location === this . CombatLocation) {
					this . Report . push ({text: `${Ship . name} destroyed during Exodus.`, colour: Ship . colour});
					this . DeleteShip (this . Turns [0], ship);
				}
			}
		}
	}
	for (var ship in this . races [this . Turns [0]] . ships) {
		var Ship = this . races [this . Turns [0]] . ships [ship];
		if (Ship . location === this . CombatLocation) {
			// console . log (`${Ship . name} for exodus.`);
			if (Ship . WarpGenerator) {
				var Location = this . stars [this . CombatLocation] . location;
				var shift = [{x: 1, y: 0}, {x: 0, y: 1}, {x: -1, y: 0}, {x: 0, y: -1}, {x: 1, y: 1}, {x: -1, y: -1}];
				shift = shift [Math . floor (Math . random () * shift . length)];
				Ship . location = {x: pmod (Location . x + shift . x, this . size . width), y: pmod (Location . y + shift . y, this . size . height)};
				for (var star in this . stars) {
					var SL = this . stars [star] . location;
					if (SL . x === Ship . location . x && SL . y === Ship . location . y) Ship . location = star;
				}
			}
		}
	}
	this . CombatShip = null;
};
