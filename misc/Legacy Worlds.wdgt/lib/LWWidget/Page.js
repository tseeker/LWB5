LWWidget.Page = Base.Comp.inherits({

	constructor: function () {
		this.base();

		this.addEvent('Show');
		this.addEvent('Hide');
	},

	writeContents: function (contents) {
		var e = document.getElementById('mtContents');
		if (e) {
			e.innerHTML = contents;
		}
	}

});
