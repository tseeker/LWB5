LWWidget.Data.Leaf = LWWidget.Data.inherits({

	constructor: function (name, text) {
		this.base(name);
		this.text = text;
	},

	getText: function () {
		return this.text;
	}

});
