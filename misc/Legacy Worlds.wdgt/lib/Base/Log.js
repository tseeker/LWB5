Base.Log = Base.inherits({

	constructor: function (maxSize) {
		this.base();
		this.log	= new Array();
		this.maxSize	= maxSize ? maxSize : 4096;
	},

	insert: function (line) {
		if (!line) {
			return;
		}
		if (this.log.length == this.maxSize) {
			this.log.shift();
		}
		this.log.push(line);
	},

	flush: function () {
		this.log = new Array();
	}

}, {

	singleton:	null,

	initLog: function (maxSize) {
		if (!this.singleton) {
			this.singleton = new this(maxSize);
		}
	},

	write: function (line) {
		if (this.singleton) {
			this.singleton.insert(line);
		}
	},

	get: function () {
		return (this.singleton ? this.singleton.log : new Array());
	},

	flush: function () {
		if (!this.singleton) {
			return new Array();
		}
		var _r = this.singleton.log;
		this.singleton.flush();
		return _r;
	}

});
