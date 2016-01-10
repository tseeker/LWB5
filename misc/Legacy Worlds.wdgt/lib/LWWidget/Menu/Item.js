LWWidget.Menu.Item = Base.Comp.inherits({

	constructor: function (name, text, clickable, selectable, separator) {
		this.base();

		this.name = name;
		this.text = text;
		this.clickable = !!clickable;
		this.selectable = this.clickable && !!selectable;
		this.separator = (typeof separator == 'string') ? separator : '';
		this.selected = false;

		this.addEvent('Click');
	},

	draw: function () {
		var pfx, sfx;
		if (this.clickable && !this.selected) {
			pfx = '<a href="#" onclick="Base.Comp.get(' + this._cid + ').click();return false">';
			sfx = '</a>';
		} else if (this.selectable && this.selected) {
			pfx = '<b>';
			sfx = '</b>';
		} else {
			pfx = sfx = '';
		}

		return pfx + this.text + sfx;
	},

	click: function () {
		if (this.selectable) {
			this.selected = true;
		}
		this.onClick(this);
	}

});
