LWWidget.Data = Base.inherits({

	constructor: function (name) {
		this.base();
		this.name = name;
		this.attributes = new Base.Util.Hashtable();
	},

	setAttribute: function (name, value) {
		this.attributes.put(name, value);
	},

	getAttribute: function (name) {
		return (this.attributes.containsKey(name) ? this.attributes.get(name) : null);
	},

	getChildren: function (name) {
		return new Array();
	},

	getText: function () {
		return "";
	}

});
