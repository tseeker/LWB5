LWWidget.Data.Node = LWWidget.Data.inherits({

	constructor: function (name) {
		this.base(name);
		this.contents = new Array();
	},

	addChild: function (child) {
		if (child instanceof LWWidget.Data.Node || child instanceof LWWidget.Data.Leaf) {
			this.contents.push(child);
		}
	},


	getChildren: function (name) {
		var _c = new Array();
		for (var i in this.contents) {
			if (this.contents[i] && this.contents[i].name == name) {
				_c.push(this.contents[i]);
			}
		}
		return _c;
	}

});
