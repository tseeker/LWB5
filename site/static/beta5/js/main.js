function	formatNumber(n)
{
	var l = n.length, m = l % 3, c = (l - m) / 3, v = ((m > 0) ? n.substr(0, m) : ''), z;
	for	(z=0;z<c;z++)
	{
		if	(v != "")
			v += ',';
		v += n.substr(m+(z*3), 3);
	}
	return	v;
}


//--------------------------------------------------------------------------------------
// ARRAY EXTENSION
//--------------------------------------------------------------------------------------

Array_search = function (value) {
	var found=null;
	if (value != null)
		for (var i in this) {
			if (this[i]==value) {
				found=i;
				break;
			}
		}
	return found;
}


//--------------------------------------------------------------------------------------
// HASHTABLE IMPLEMENTATION
//--------------------------------------------------------------------------------------

function Hashtable()
{
	this.clear		= Hashtable_clear;
	this.containsKey	= Hashtable_containsKey;
	this.containsValue	= Hashtable_containsValue;
	this.get		= Hashtable_get;
	this.isEmpty		= Hashtable_isEmpty;
	this.keys		= Hashtable_keys;
	this.put		= Hashtable_put;
	this.remove		= Hashtable_remove;
	this.size		= Hashtable_size;
	this.toString		= Hashtable_toString;
	this.values		= Hashtable_values;
	this.hashtable		= new Array();
}

function	Hashtable_clear() { this.hashtable = new Array(); }
function	Hashtable_get(key) { return this.hashtable[key]; }
function	Hashtable_isEmpty() { return (parseInt(this.size())==0) ? true:false; }

function	Hashtable_containsKey(key)
{
	var exists=false;
	for	(var i in this.hashtable)
	{
		if	(i==key&&this.hashtable[i]!=null)
		{
			exists=true;
			break;
		}
	}
	return exists;
}

function	Hashtable_containsValue(value)
{
	var contains=false;
	if	(value!=null)
		for	(var i in this.hashtable)
		{
			if	(this.hashtable[i]==value)
			{
				contains=true;
				break;
			}
		}
	return contains;
}

function	Hashtable_keys()
{
	var keys=new Array();
	for	(var i in this.hashtable)
	{
		if(this.hashtable[i]!=null)
			keys.push(i);
	}
	return keys;
}

function	Hashtable_put(key,value)
{
	if	(key==null||value==null)
		throw "NullPointerException {"+key+"},{"+value+"}";
	else
		this.hashtable[key]=value;
}

function	Hashtable_remove(key)
{
	var rtn=this.hashtable[key];
	this.hashtable[key]=null;
	return rtn;
}

function	Hashtable_size()
{
	var size=0;
	for	(var i in this.hashtable)
	{
		if	(this.hashtable[i]!=null)
			size++;
	}
	return size;
}

function	Hashtable_toString()
{
	var result="";
	for	(var i in this.hashtable)
	{
		if	(this.hashtable[i]!=null)
			result+="{"+i+"},{"+this.hashtable[i]+"}\n";
	}
	return result;
}

function	Hashtable_values()
{
	var values=new Array();
	for	(var i in this.hashtable)
	{
		if	(this.hashtable[i]!=null)
			values.push(this.hashtable[i]);
	}
	return values;
}


//----------------------------------------------------------------------------------------------
// COMPONENTS
//----------------------------------------------------------------------------------------------

Component = function(isBlock) {
	if	(typeof isBlock == "undefined")
		return;

	this.id = ++Component.lId;
	Component.list[this.id] = this;
	this.slots = new Hashtable();
	this.events = new Hashtable();
	this.children = new Hashtable();
	this.visible = false;
	this.isBlock = isBlock;

	this.newSlot('show');
	this.newSlot('hide');
	this.newSlot('drawAll');
	this.newSlot('draw');

	this.newEvent('Show', true);
	this.newEvent('Hide', true);

	this.bindEvent('Show','drawAll');
}
Component.list = new Array();
Component.lId = -1;
Component.prototype.setVisible = function(v) {
	this.visible=v;
	var i,k=this.children.keys();
	for	(i=0;i<k.length;i++)
		this.children.get(k[i]).setVisible(v);
}
Component.prototype.draw = function() { }
Component.prototype.drawAll = function() {
	if	(!this.visible)
		return;

	var i,k=this.children.keys();
	this.draw();
	for	(i=0;i<k.length;i++)
		this.children.get(k[i]).drawAll();
}
Component.prototype.show = function() {
	this.setVisible(true);
	this.onShow();
}
Component.prototype.hide = function() {
	this.setVisible(false);
	this.onHide();
}
Component.prototype.getDocID = function() { return "cmpid" + this.id; }
Component.prototype.getElement = function() { return document.getElementById(this.getDocID()); }
Component.prototype.jsCode = function() { return "Component.list[" + this.id + "]"; }
Component.prototype.html = function(cl) {
	return (this.isBlock ? '<div' : '<span') + ' id="' + this.getDocID() + '"'
			+ (typeof cl == "undefined" ? "" : (' class="' + cl + '"'))
			+ '>&nbsp;</' + (this.isBlock ? 'div' : 'span') + '>';
}
Component.prototype.newSlot = function(name) {
	if	(this.slots.containsKey(name))
		return;
	var	s = new Component.Slot(name,this);
	this.slots.put(name, s);
}
Component.prototype.newEvent = function(name, propagate) {
	if	(this.events.containsKey(name))
		return;

	var	e = new Component.Event(name, propagate);
	this.events.put(name, e);

	var	f = function() { this.triggerEvent(name,arguments); };
	eval("this.on"+name+'=f');
}
Component.prototype.bindEvent = function(eName,sName,cmp) {
	var	e = this.events.get(eName), c = (cmp ? cmp : this), s = c.slots.get(sName);
	if	(!(e && s))
		return;
	e.bind(s);
}
Component.prototype.detachEvent = function(eName,sName,cmp) {
	var	e = this.events.get(eName), c = (cmp ? cmp : this), s = c.slots.get(sName);
	if	(!(e && s))
		return;
	e.detach(s);
}
Component.prototype.triggerEvent = function(eName,args) {
	var	e = this.events.get(eName);
	if	(!e)
		return;

	var i, n = new Array();
	for     (i=0;i<args.length;i++)
		n.push('args[' + i + ']');
	eval('e.trigger(' + n.join(',') + ')');

	if	(!e.propagate)
		return;
	var k=this.children.keys();
	for	(i=0;i<k.length;i++)
		this.children.get(k[i]).triggerEvent(eName, args);
}
Component.prototype.addChild = function(cmp) {
	if	(!cmp || cmp.id == this.id)
		return;
	cmp.setVisible(this.visible);
	this.children.put(cmp.id, cmp);
	this.drawAll();
}
Component.prototype.removeChild = function(cmp) {
	if	(!cmp || cmp.id == this.id || !this.children.containsKey(cmp.id))
		return;
	this.children.remove(cmp.id);
	cmp.setVisible(false);
	this.drawAll();
}
Component.prototype.hasChild = function(cmp) {
	if	(!cmp)
		return	false;
	return	this.children.containsKey(cmp.id);
}


Component.Slot = function (name, component) {
	this.name	= name;
	this.component	= component;
	this.id		= ++Component.Slot.lId;
}
Component.Slot.lId = -1;
Component.Slot.prototype.call = function() {
	var i, n = new Array();
	for	(i=0;i<arguments.length;i++)
		n.push('arguments[' + i + ']');
	eval('this.component.'+this.name+'('+n.join(',')+')');
}


Component.Event = function (name, propagate) {
	this.name	= name;
	this.propagate	= propagate;
	this.bindings	= new Hashtable();
}
Component.Event.prototype.bind = function(slot) {
	if	(!this.bindings.containsKey(slot.id))
		this.bindings.put(slot.id, slot);
}
Component.Event.prototype.detach = function(slot) {
	if	(this.bindings.containsKey(slot.id))
		this.bindings.remove(slot.id);
}
Component.Event.prototype.trigger = function() {
	var i, n = new Array();
	for	(i=0;i<arguments.length;i++)
		n.push('arguments[' + i + ']');
	var k = this.bindings.keys(), s = n.join(',');
	for	(i=0;i<k.length;i++)
		eval('this.bindings.get('+k[i]+').call('+s+')');
}



//----------------------------------------------------------------------------------------------
// TIMER COMPONENT
//----------------------------------------------------------------------------------------------

Component.Timer = function(miliseconds,repeat) {
	if	(typeof miliseconds == 'undefined')
		return;

	Component.apply(this, [false]);

	this.miliseconds	= miliseconds;
	this.repeat		= repeat;

	this.timer		= null;
	this.lastAt		= 0;
	this.pausedAt		= 0;
	this.locked		= false;
	this.running		= false;

	this.newEvent('Tick');
}
Component.Timer.prototype = new Component;
Component.Timer.prototype.start = function() {
	while	(this.locked) ;
	this.locked = true;
	if	(!this.running)
	{
		this.running = true;
		this.lastAt = new Date().getTime();
		eval(this.getCode());
	}
	this.locked = false;
}
Component.Timer.prototype.stop = function() {
	while	(this.locked) ;
	this.locked = true;
	if	(this.running)
	{
		clearTimeout(this.timer);
		this.lastAt = this.pausedAt = 0;
		this.running = false;
	}
	this.locked = false;
}
Component.Timer.prototype.restart = function() {
	while	(this.locked) ;
	this.locked = true;
	if	(this.running)
	{
		clearTimeout(this.timer);
		this.pausedAt = 0;
	}
	else
		this.running = true;
	this.lastAt = new Date().getTime();
	eval(this.getCode());
	this.locked = false;
}
Component.Timer.prototype.pause = function () {
	while	(this.locked) ;
	this.locked = true;
	if	(this.running)
	{
		if	(this.pausedAt == 0)
		{
			this.pausedAt = new Date().getTime();
			if	(this.timer)
				clearTimeout(this.timer);
		}
		else
		{
			eval(this.getCode());
			this.pausedAt = 0;
		}
	}
	this.locked = false;
}
Component.Timer.prototype.tick = function() {
	while	(this.locked) ;
	this.locked = true;
	var	re;
	if	(re=(this.running && !this.pausedAt))
	{
		if	(this.repeat)
		{
			eval(this.getCode());
			this.lastAt = new Date().getTime();
		}
		else
		{
			this.running = false;
			this.timer = null;
			this.lastAt = 0;
		}
	}
	this.locked = false;
	if	(re)
		this.onTick();
}
Component.Timer.prototype.getCode = function() {
	return	this.jsCode() + '.timer=setTimeout("' + this.jsCode() + '.tick()", '
			+ (this.pausedAt ? (this.miliseconds - this.pausedAt + this.lastAt) : this.miliseconds)
			+ ")";
}



//----------------------------------------------------------------------------------------------
// AJAX COMPONENT
//----------------------------------------------------------------------------------------------

Component.Ajax = function(callName) {
	if	(typeof callName == 'undefined')
		return;

	Component.apply(this, [false]);
	this.callName = callName;
	this.newEvent('Returned');
}
Component.Ajax.prototype = new Component;
Component.Ajax.prototype.call = function() {
	eval('Array.prototype.push.apply(arguments,[function(d){'+this.jsCode()+'.onReturned(d)}])');
	rpc_doCall(this.callName, arguments);
}



//----------------------------------------------------------------------------------------------
// FORM ELEMENT COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form = {}
Component.Form.Element = function(type, hasContents) {
	if	(typeof type == "undefined")
		return;

	Component.apply(this, [false]);
	this.type		= type;
	this.hasContents	= hasContents;
	this.attributes		= new Hashtable();
	this.htmlEvents		= new Array();
	this.value		= null;
	this.htmlElement	= null;

	this.setAttribute('name', this.getDocID() + 'f');
	this.setAttribute('id', this.getDocID() + 'f');

	this.newSlot('hideElement');
	this.bindEvent('Hide', 'hideElement');

	this.addHTMLEvent('MouseOver');
	this.addHTMLEvent('MouseOut');
}
Component.Form.Element.prototype = new Component;
Component.Form.Element.prototype.addChild = function () { }
Component.Form.Element.prototype.hideElement = function () { this.htmlElement = null; }
Component.Form.Element.prototype.setAttribute = function (name,def) { this.attributes.put(name.toLowerCase(), def); this.drawAll(); }
Component.Form.Element.prototype.addHTMLEvent = function (name) { this.htmlEvents.push(name); this.newEvent(name); this.drawAll(); }
Component.Form.Element.prototype.getTag = function () {
	var	s = '<' + this.type;
	var	k = this.attributes.keys();
	for	(var a in k)
	{
		var	av = this.attributes.get(k[a]);
		if	(av == '')
			continue;
		s += ' ' + k[a] + '="' + av + '"';
	}

	var	jc = this.jsCode();
	for	(var ei in this.htmlEvents)
	{
		var	ev = this.htmlEvents[ei];
		s += ' on' + ev.toLowerCase() + '="' + jc + '.on' + ev + '(event)"';
	}
	return	s + (this.hasContents ? '>' : '/>');
}
Component.Form.Element.prototype.draw = function () {
	var	e = this.getElement();
	if	(!e)
		return;

	var	s = this.getTag();
	if	(this.hasContents)
		s += '</' + this.type + '>';

	e.innerHTML = s;
	this.htmlElement = document.getElementById(this.attributes.get('id'));
	this.updateValue();
}
Component.Form.Element.prototype.updateValue = function (nv) {
	if	(typeof nv != "undefined")
		this.value = nv;
	if	(this.htmlElement)
		this.htmlElement.value = this.value;
}



//----------------------------------------------------------------------------------------------
// LABEL COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form.Label = function(target, text, key) {
	if	(typeof target == "undefined")
		return;

	Component.apply(this, [false]);
	this.target	= target;
	this.text	= text;
	this.key	= key;
}
Component.Form.Label.prototype = new Component;
Component.Form.Label.prototype.addChild = function () { }
Component.Form.Label.prototype.draw = function () {
	var	 e = this.getElement();
	if	(!e)
		return;
	e.innerHTML = '<label for="' + this.target.getDocID() + 'f"'
		+ (this.key ? (' key="' + this.key + '"') : '')
		+ '>' + this.text + '</label>';
}



//----------------------------------------------------------------------------------------------
// TEXT INPUT COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form.Text = function (defValue) {
	if	(typeof defValue == 'undefined')
		return;

	Component.Form.Element.apply(this, ['input type="text"', false]);

	var	ev = ['Click', 'KeyUp', 'KeyDown', 'KeyPress', 'Paste'];
	this.newSlot('textEvent');
	for	(var i in ev)
	{
		this.addHTMLEvent(ev[i]);
		this.bindEvent(ev[i], 'textEvent');
	}

	this.timer = new Component.Timer(500, false);
	this.newSlot('timeout');
	this.timer.bindEvent('Tick', 'timeout', this);

	this.oldValue = this.value = defValue;
	this.newEvent('TextChanged');
}
Component.Form.Text.prototype = new Component.Form.Element;
Component.Form.Text.prototype.textEvent = function() {
	if	(this.htmlElement)
		this.value = this.htmlElement.value;
	this.timer.restart();
}
Component.Form.Text.prototype.timeout = function() {
	if	(this.htmlElement)
		this.value = this.htmlElement.value;
	if	(this.value != this.oldValue)
	{
		this.oldValue = this.value;
		this.onTextChanged(this.value);
	}
}
Component.Form.Text.prototype.setSize = function (sz) {
	this.setAttribute('size', (typeof sz == 'undefined') ? '' : sz);
}
Component.Form.Text.prototype.setMaxLength = function (ml) {
	if	(typeof ml != "undefined" && this.value.length > ml)
	{
		this.timer.stop();
		this.value = this.value.substr(0, ml);
	}
	this.setAttribute('maxlength', (typeof ml == 'undefined') ? '' : ml);
}



//----------------------------------------------------------------------------------------------
// CHECKBOX COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form.Checkbox = function (value,name) {
	if	(typeof value == 'undefined')
		return;

	Component.Form.Element.apply(this, ['input type="checkbox"', false]);

	this.newSlot('check');
	this.addHTMLEvent('Click');
	this.bindEvent('Click', 'check');

	this.newEvent('Checked');
	this.newEvent('Unchecked');

	this.value = value;
	this.checked = false;

	this.setAttribute('name', this.group = name);
}
Component.Form.Checkbox.prototype = new Component.Form.Element;
Component.Form.Checkbox.prototype.check = function(evt) {
	this.checked = !this.checked;

	if	(typeof evt == "undefined")
		this.drawAll();

	if (this.checked) {
		this.attributes.put('checked', 'checked');
		this.onChecked(this.value, this.group);
	} else {
		this.attributes.remove('checked');
		this.onUnchecked(this.value, this.group);
	}
}



//----------------------------------------------------------------------------------------------
// RADIO BUTTON COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form.Radio = function (value,name) {
	if	(typeof value == 'undefined')
		return;

	Component.Form.Element.apply(this, ['input type="radio"', false]);

	this.newSlot('check');
	this.addHTMLEvent('Click');
	this.bindEvent('Click', 'check');

	this.newEvent('Checked');
	this.newEvent('Unchecked');

	this.value = value;
	this.checked = false;

	this.setAttribute('name', this.group = name);
	with	(Component.Form.Radio.groups)
	{
		if	(!get(name))
			put(name, new Array());
		get(name).push(this);
	}
}
Component.Form.Radio.prototype = new Component.Form.Element;
Component.Form.Radio.groups = new Hashtable();
Component.Form.Radio.prototype.check = function(evt) {
	var	k = Component.Form.Radio.groups.keys();
	for	(var i in k)
	{
		var	c = Component.Form.Radio.groups.get(k[i]);
		if	(c.id != this.id && c.checked)
		{
			c.checked = false;
			c.attributes.put('checked', '');
			if	(typeof evt == "undefined")
				c.drawAll();
			c.onUnchecked(c.group, c.value);
			break;
		}
	}
	this.checked = true;
	this.attributes.put('checked', 'checked');
	if	(typeof evt == "undefined")
		this.drawAll();
	this.onChecked(this.value, this.group);
}



//----------------------------------------------------------------------------------------------
// DROP-DOWN COMPONENT
//----------------------------------------------------------------------------------------------

Component.Form.DropDown = function (emptyValue,emptyLabel) {
	if	(typeof emptyValue == 'undefined')
		return;

	Component.Form.Element.apply(this, ['select', true]);

	this.newSlot('htmlSelect');
	this.addHTMLEvent('Change');
	this.bindEvent('Change', 'htmlSelect');

	this.newEvent('Selected');
	this.options	= new Array();
	this.byValue	= new Hashtable();

	if	(typeof emptyValue != 'string')
		emptyValue = emptyValue.toString();
	this.emptyValue = emptyValue;
	this.emptyLabel = emptyLabel;
	this.selected	= emptyValue;
}
Component.Form.DropDown.prototype = new Component.Form.Element;
Component.Form.DropDown.prototype.clearOptions = function(redraw) {
	this.byValue.clear();
	this.options = new Array();
	this.selected = this.emptyValue;
}
Component.Form.DropDown.prototype.appendOption = function(value,text) {
	if	(typeof value != 'string')
		value = value.toString();

	if	(this.byValue.containsKey(value))
		return;

	this.options.push([value, text]);
	this.byValue.put(value, this.options.length - 1);

	if	(this.selected == this.emptyValue)
		this.selected = value;
}
Component.Form.DropDown.prototype.finalize = function() {
	this.drawAll();
	this.onSelected(this.selected);
}
Component.Form.DropDown.prototype.htmlSelect = function() {
	if	(this.htmlElement)
	{
		var	se = this.htmlElement.selectedIndex;
		this.selected = this.htmlElement.options[se].value;
	}
	this.onSelected(this.selected);
}
Component.Form.DropDown.prototype.select = function(v) {
	if	(typeof v != 'string')
		v = v.toString();

	if	(!this.options.length || ! this.byValue.containsKey(v) || this.selected == v)
		return;

	this.selected = v;
	this.drawAll();
	this.onSelected(this.selected);
}
Component.Form.DropDown.prototype.updateValue = function() { }
Component.Form.DropDown.prototype.draw = function() {
	var	e = this.getElement();
	if	(!e)
		return;

	var	s = '';
	if	(this.options.length)
		for	(var i in this.options)
			s += '<option value="' + this.options[i][0] + '"'
				+ (this.selected == this.options[i][0] ? ' selected="selected"' : '')
				+ '>' + this.options[i][1] + '</option>';
	else
		s = '<option value="' + this.emptyValue + '">' + this.emptyLabel + '</option>';

	e.innerHTML = this.getTag() + s + '</select>';
	this.htmlElement = document.getElementById(this.attributes.get('id'));
}



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT
//----------------------------------------------------------------------------------------------
Component.Listing = function(rpcName, updateFreq, dataType, cl, rClass, hClass)
{
	if	(typeof rpcName == 'undefined')
		return;

	Component.apply(this, [false]);

	this.pageCtrl	= null;
	this.searchCtrl	= null;
	this.pageSzCtrl	= null;

	this.dataCache	= new Component.Listing.DataCache(this, rpcName, updateFreq, dataType);
	this.addChild(this.dataCache);

	this.fields	= new Hashtable();
	this.fLayout	= { };
	this.rows	= 0;
	this.cols	= 0;
	this.mClass	= cl;
	this.rClass	= (rClass ? rClass : '');
	this.hClass	= (hClass ? hClass : '');
	this.finalized	= false;
	this.sortField	= -1;
	this.sortDir	= false;

	this.notFoundText	= Component.Listing.notFoundText;
	this.loadingText	= Component.Listing.loadingText;
	this.errorText		= Component.Listing.errorText;
}
Component.Listing.prototype = new Component;
Component.Listing.prototype.setPageController = function(c,itg) {
	if	(this.pageCtrl)
		// FIXME, should detach the old controller
		return;
	this.pageCtrl = c;
	this.pageCtrl.bindEvent('PageChanged', 'pageChanged', this.dataCache);
	if	(itg)
		this.addChild(this.pageCtrl);
	this.dataCache.pageChanged(this.pageCtrl.currentPage);
}
Component.Listing.prototype.setPageSizeController = function(c,itg) {
	if	(this.pageSzCtrl)
		// FIXME, should detach the old controller
		return;
	this.pageSzCtrl = c;
	this.pageSzCtrl.bindEvent('PageSizeChanged', 'pageSizeChanged', this.dataCache);
	if	(itg)
		this.addChild(this.pageSzCtrl);
	this.dataCache.pageSizeChanged(this.pageSzCtrl.currentSize);
}
Component.Listing.prototype.setSearchController = function(c,itg) {
	if	(this.searchCtrl)
		// FIXME, should detach the old controller
		return;
	this.searchCtrl = c;
	this.searchCtrl.bindEvent('SearchChanged', 'searchChanged', this.dataCache);
	if	(itg)
		this.addChild(this.searchCtrl);
	this.dataCache.searchChanged(this.searchCtrl.getParameter());
}
Component.Listing.prototype.addField = function(fld) {
	var	x,y,xe=fld.x+fld.width,ye=fld.y+fld.height;
	for	(y=fld.y;y<ye;y++)
		for	(x=fld.x;x<xe;x++)
			if	(this.fLayout[x+'_'+y])
				return;
	this.fields[fld.id] = fld;
	fld.component = this;
	if	(fld.sortable) {
		if	(! this.dataCache.sortField)
			this.dataCache.sortField = fld;
		fld.bindEvent('Click', 'sortChanged', this.dataCache);
	}
	for	(y=fld.y;y<ye;y++)
		for	(x=fld.x;x<xe;x++)
			this.fLayout[x+'_'+y] = fld;
}
Component.Listing.prototype.finalizeLayout = function() {
	var	mx = 0, my = 0, x, y;
	for	(var c in this.fLayout)
	{
		var	cs=c.split('_');
		x=parseInt(cs[0],10); y=parseInt(cs[1],10);
		if	(x>mx)
			mx = x;
		if	(y>my)
			my = y;
	}

	this.rows = my+1; this.cols = mx+1;
	for	(y=0;y<this.rows;y++)
		for	(x=0;x<this.cols;x++)
			if	(!this.fLayout[x+'_'+y])
				this.fLayout[x+'_'+y] = new Component.Listing.Spacer(x,y);

	this.finalized = true;
}
Component.Listing.prototype.setMaxPage = function (v) {
	if	(this.pageCtrl)
		this.pageCtrl.changeMaxPage(v);
}
Component.Listing.prototype.draw = function () {
	var	e = this.getElement();
	if	(!e)
		return;

	// Draw the listing's header including placeholders for the controllers if that is necessary
	var	s = '';
	var	hp = this.hasChild(this.pageCtrl), hs = this.hasChild(this.searchCtrl),
			hps = this.hasChild(this.pageSzCtrl), hh = (hp||hs||hps);
	if	(hh)
	{
		s += '<table class="' + this.hClass + '">';
		if	(hp||hps)
		{
			s += '<tr><td style="width:50%;text-align:center"';
			if	(hp)
				s += ' id="' + this.pageCtrl.getDocID() + '"';
			s += '>&nbsp;</td><td style="width:50%;text-align:center"';
			if	(hps)
				s += ' id="' + this.pageSzCtrl.getDocID() + '"';
			s += '>&nbsp;</td></tr>';
		}
		if	(hs)
			s += '<tr><td style="text-align:center" id="' + this.searchCtrl.getDocID() + '"'
				+ ((hp||hps)?' colspan="2"' : '') + '>&nbsp;</td></tr>';
		s += '</td></tr></table>';
	}
	s += '<div id="' + this.id + 'data"></div>';

	e.innerHTML = s;
}
Component.Listing.prototype.getHeaders = function () {
	var	s = '<table class="'+this.mClass+'">';
	var	lId = -1, x, y, rc = (this.rClass != '' ? (" class='" + this.rClass + "'") : "");
	for	(y=0;y<this.rows;y++)
	{
		if	(y == 0)
			s += '<tr><td><table'+rc+'>';
		s += '<tr>';
		for	(x=0;x<this.cols;x++)
		{
			var	f = this.fLayout[x+'_'+y];
			if	(f.id == lId || y != f.y)
				continue;
			lId = f.id;
			s += f.getTitle();
		}
		s += '</tr>' + ((y == this.rows - 1) ? '</table></td></tr>' : '');
	}
	return	s;
}



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - PAGE CONTROLLER
//----------------------------------------------------------------------------------------------

Component.Listing.PageController = function(text) {
	if	(typeof text == 'undefined')
		return;

	Component.apply(this, [false]);

	this.text = text;
	this.maxPage = 0;

	this.addChild(this.select = new Component.Form.DropDown('', '-'));
	this.newSlot('selected');
	this.select.bindEvent('Selected','selected',this);

	this.newSlot('changePage');
	this.newSlot('changeMaxPage');

	this.newEvent('PageChanged');
}
Component.Listing.PageController.prototype = new Component;
Component.Listing.PageController.prototype.selected = function(v) {
	this.changePage(parseInt(v,10));
}
Component.Listing.PageController.prototype.changePage = function(np) {
	var	op = this.currentPage;
	this.currentPage = (np > this.maxPage) ? this.maxPage : np;
	if	(op != this.currentPage) {
		this.select.select(this.currentPage);
		this.onPageChanged(this.currentPage);
	}
	this.drawAll();
}
Component.Listing.PageController.prototype.changeMaxPage = function(nmp) {
	var	omp = this.maxPage, ocp = this.currentPage;
	this.maxPage = nmp;
	if	(omp != nmp)
	{
		var	i;
		this.select.clearOptions();
		for	(i=0;i<nmp+1;i++)
			this.select.appendOption(i, i+1);

		this.changePage(this.currentPage);
		if	(this.currentPage == ocp)
			this.drawAll();
	}
}
Component.Listing.PageController.prototype.draw = function() {
	var	e = this.getElement(), id = this.getDocID();
	if	(!e)
		return;

	var	m, s2;
	if	(this.maxPage <= 0) {
		m = 1;
		s2 = '1';
	} else {
		m = (this.maxPage + 1);
		s2 = this.select.html();
	}

	var	s = this.text.replace(/__MP__/, m.toString());
	e.innerHTML = s.replace(/__CP__/, s2);
}



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - PAGE SIZE CONTROLLER
//----------------------------------------------------------------------------------------------

Component.Listing.PageSizeController = function(text, minVal, maxVal, increment) {
	if	(typeof text == 'undefined')
		return;

	Component.apply(this, [false]);

	this.text = text;
	this.currentSize = minVal;

	this.addChild(this.select = new Component.Form.DropDown('', '-'));
	for (var i=minVal;i<=maxVal;i+=increment)
		this.select.appendOption(i, i);
	this.newSlot('selected');
	this.select.bindEvent('Selected','selected',this);

	this.newSlot('changePageSize');
	this.newEvent('PageSizeChanged');
}
Component.Listing.PageSizeController.prototype = new Component;
Component.Listing.PageSizeController.prototype.selected = function(v) { this.changePageSize(parseInt(v,10)); }
Component.Listing.PageSizeController.prototype.changePageSize = function(nps) {
	var	os = this.currentSize;
	this.currentSize = nps;
	if	(os != this.currentSize) {
		this.select.select(this.currentSize);
		this.onPageSizeChanged(this.currentSize);
	}
	this.drawAll();
}
Component.Listing.PageSizeController.prototype.draw = function() {
	var	e = this.getElement(), id = this.getDocID();
	if	(!e)
		return;
	e.innerHTML = this.text.replace(/__PS__/, this.select.html());
}



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - SEARCH CONTROLLER
//----------------------------------------------------------------------------------------------

Component.Listing.SearchController = function(text) {
	if	(typeof text == 'undefined')
		return;

	Component.apply(this, [false]);

	this.newSlot('textChanged');
	this.addChild(this.textField = new Component.Form.Text(''));
	this.textField.bindEvent('TextChanged', 'textChanged', this);
	this.text = text;

	this.newSlot('modeChanged');
	this.modes = new Array();
	this.mode = -1;

	this.newEvent('SearchChanged');
}
Component.Listing.SearchController.prototype = new Component;
Component.Listing.SearchController.prototype.addMode = function (value, text) {
	if	(typeof this.modes[value] != "undefined")
		return;

	var	c = new Component.Form.Radio(value, this.getDocID() + 'mode');
	c.bindEvent('Checked', 'modeChanged', this);
	this.addChild(c);

	var	c2 = new Component.Form.Label(c, text);
	this.addChild(c2);

	this.modes[value] = [c,c2];
}
Component.Listing.SearchController.prototype.setText = function (text) {
	if	(text == this.textField.value)
		return;
	this.textField.updateValue(text);
	this.onSearchChanged(this.mode + '!' + text);
}
Component.Listing.SearchController.prototype.setMode = function (mode) {
	if	(typeof this.modes[mode] == "undefined" || this.mode == mode)
		return;
	this.modes[mode][0].check();
}
Component.Listing.SearchController.prototype.draw = function () {
	var	e = this.getElement(), id = this.getDocID();
	if	(!e)
		return;

	var	s, s2, cd;
	s = this.text.replace(/__TF__/, this.textField.html());

	if	(this.modes.length > 1)
	{
		s2 = new Array();
		for	(var i in this.modes)
			s2.push(this.modes[i][0].html() + this.modes[i][1].html() + ' ');
		s = s.replace(/__MS__/, s2.join(' '));
	}

	e.innerHTML = s;
}
Component.Listing.SearchController.prototype.textChanged = function (text) { this.onSearchChanged(this.mode + '!' + text); }
Component.Listing.SearchController.prototype.modeChanged = function (mode) { this.onSearchChanged((this.mode=mode) + '!' + this.textField.value); }
Component.Listing.SearchController.prototype.getParameter = function () { return this.mode + '!' + this.textField.value; }



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - SPACERS
//----------------------------------------------------------------------------------------------

Component.Listing.Spacer = function (x,y,fClass) {
	if	(typeof x == 'undefined')
		return;
	Component.apply(this, [true]);
	this.fClass = fClass ? fClass : '';
	this.attrCode = null;
	this.x = x; this.y = y;
	this.width = this.height = 1;
	this.sortable = false;
	this.component = null;
	this.newEvent('Click');
}
Component.Listing.Spacer.prototype = new Component;
Component.Listing.Spacer.prototype.getAttributes = function() {
	if	(typeof this.attrCode == 'string')
		return	this.attrCode;
	var	s = '';
	if	(this.fClass != '')
		s += ' class="' + this.fClass + '"';
	return	(this.attrCode = s);
}
Component.Listing.Spacer.prototype.getData = function() {
	return	'<td' + this.getAttributes() + '>&nbsp;</td>';
}
Component.Listing.Spacer.prototype.getTitle = function() {
	return	'<th' + this.getAttributes() + '>&nbsp;</th>';
}



//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - FIELDS
//----------------------------------------------------------------------------------------------

Component.Listing.Field = function (name,sortable,dataField,x,y,w,h,fClass) {
	if	(typeof name == 'undefined')
		return;
	Component.Listing.Spacer.apply(this, [x,y,fClass]);
	this.name = name;
	this.sortable = sortable;
	this.dataField = dataField;
	this.width = w; this.height = h;
}
Component.Listing.Field.prototype = new Component.Listing.Spacer;
Component.Listing.Field.prototype.getAttributes = function() {
	if	(typeof this.attrCode == 'string')
		return	this.attrCode;
	var	s = '';
	if	(this.fClass != '')
		s += ' class="' + this.fClass + '"';
	if	(this.width != 1)
		s += ' colspan="' + this.width + '"';
	if	(this.height != 1)
		s += ' rowspan="' + this.height + '"';
	return	(this.attrCode = s);
}
Component.Listing.Field.prototype.getContents = function(d) {
	var	v;
	eval('v=d.' + this.dataField);
	if	(typeof v != 'string')
		v = v.toString();
	return	v;
}
Component.Listing.Field.prototype.getData = function(d) {
	return	'<td' + this.getAttributes() + '>' + this.getContents(d) + '</td>';
}
Component.Listing.Field.prototype.getTitle = function() {
	var	s = '<th' + this.getAttributes();
	var	srt = (this.sortable && this.component.dataCache.sortField
			&& this.component.dataCache.sortField.id == this.id);

	if	(this.sortable)
		s += ' onClick="' + this.jsCode() + '.onClick(Component.list[' + this.id + '])"';
	s += '>';
	if	(srt)
		s += '<b>';
	s += this.name;
	if	(srt)
		s += '</b><img src="' + staticurl + '/beta5/pics/'
			+ (this.component.dataCache.sortDir ? "up" : "down")
			+ '_' + color + '.gif" alt="'
			+ (this.component.dataCache.sortDir ? "up" : "down") + '"/>';
	return	s + '</th>';
}


//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - DATA CACHE
//----------------------------------------------------------------------------------------------

Component.Listing.DataCache = function (comp, rpcName, updateFreq, dataType)
{
	Component.apply(this, [false]);

	this.newSlot('pageChanged');
	this.newSlot('pageSizeChanged');
	this.newSlot('searchChanged');
	this.newSlot('sortChanged');
	this.newSlot('pageLoaded');
	this.newSlot('checkUpdate');
	this.newSlot('initDisplay');
	this.newSlot('stopDisplay');
	this.newEvent('SettingsChanged');
	this.newEvent('Loaded');

	this.rpcName		= rpcName;
	this.dataType		= dataType;
	this.component		= comp;

	this.sortField		= null;
	this.sortDir		= false;
	this.searchParam	= '';
	this.perPage		= 10;
	this.page		= 0;
	this.current		= "";

	this.cache		= new Hashtable();
	this.timer		= new Component.Timer(updateFreq * 1000, true);

	      this.bindEvent('Hide',		'stop',			this.timer);
	      this.bindEvent('Loaded',		'drawAll');
	      this.bindEvent('SettingsChanged',	'checkUpdate');
	      this.bindEvent('SettingsChanged',	'drawAll');
	      this.bindEvent('Show',		'initDisplay');
	this.timer.bindEvent('Tick',		'checkUpdate',		this);
}
Component.Listing.DataCache.prototype = new Component;
Component.Listing.DataCache.prototype.pageChanged = function (n) {
	this.page = n;
	this.onSettingsChanged();
}
Component.Listing.DataCache.prototype.pageSizeChanged = function (n) {
	this.perPage = n;
	this.onSettingsChanged();
}
Component.Listing.DataCache.prototype.searchChanged = function (p) {
	this.searchParam = p;
	this.onSettingsChanged();
}
Component.Listing.DataCache.prototype.sortChanged = function (col) {
	if (this.sortField && col.id == this.sortField.id)
		this.sortDir = ! this.sortDir;
	else
		this.sortField = col;
	this.onSettingsChanged();
}
Component.Listing.DataCache.prototype.pageLoaded = function (id) {
	if	(this.current != id)
		return;
	this.drawAll();
	this.onLoaded();
}
Component.Listing.DataCache.prototype.checkUpdate = function () {
	if	(!this.visible)
		return;

	var	nid = this.page+'#'+this.perPage+'#'+(this.sortField?this.sortField.dataField:'')+'#'+(this.sortDir?1:0)+'#'+this.searchParam;
	var	ne;
	if	(this.cache.containsKey(nid))
		ne = this.cache.get(nid);
	else {
		ne = new Component.Listing.DataCache.Page(this, nid);
		ne.bindEvent('Loaded', 'pageLoaded', this);
		this.cache.put(nid, ne);
	}
	this.current = nid;
	ne.checkUpdate();
}
Component.Listing.DataCache.prototype.initDisplay = function () {
	if	(this.timer.running)
		return;
	this.timer.start();
	this.onSettingsChanged();
}
Component.Listing.DataCache.prototype.draw = function () {
	var	e = document.getElementById(this.component.id + 'data');
	if	(!(e && this.cache.containsKey(this.current)))
		return;

	var	s = this.component.getHeaders();
	var	p = this.cache.get(this.current);

	if	(p.loading)
		s += '<tr><td style="text-align:center;padding:10px 0px 10px 0px;">'
			+ this.component.loadingText + '</td></tr>';
	else if	(p.error)
		s += '<tr><td style="text-align:center;padding:10px 0px 10px 0px;">'
			+ this.component.errorText + '</td></tr>';
	else if	(p.data.length == 0)
		s += '<tr><td style="text-align:center;padding:10px 0px 10px 0px;">'
			+ this.component.notFoundText + '</td></tr>';
	else {
		var	rc, i;
		rc = (this.component.rClass != '' ? (" class='" + this.component.rClass + "'") : "");

		for	(i=0;i<p.data.length;i++) {
			var	lId = -1, x, y;
			for	(y=0;y<this.component.rows;y++) {
				if	(y == 0)
					s += '<tr><td style="margin:0px;padding:0px"><table'+rc+'>';
				s += '<tr' + p.data[i].getRowStyle() + '>';
				for	(x=0;x<this.component.cols;x++) {
					var	f = this.component.fLayout[x+'_'+y];
					if	(f.id == lId || y != f.y)
						continue;
					lId = f.id;
					s += f.getData(p.data[i]);
				}
				s += '</tr>' + ((y == this.component.rows - 1) ? '</table></td></tr>' : '');
			}
		}
	}

	e.innerHTML = s + '</table>';
}


//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - DATA CACHE PAGE
//----------------------------------------------------------------------------------------------
Component.Listing.DataCache.Page = function (cache, param)
{
	Component.apply(this, [false]);

	this.newSlot('dataReceived');
	this.newEvent('Loaded');

	this.cache	= cache;
	this.param	= param;
	this.loading	= true;
	this.md5	= "";
	this.error	= false;
	this.data	= new Array();
	this.call	= new Component.Ajax(this.cache.rpcName);

	this.call.bindEvent('Returned', 'dataReceived', this);
}
Component.Listing.DataCache.Page.prototype = new Component;
Component.Listing.DataCache.Page.prototype.checkUpdate = function () {
	this.loading = true;
	this.call.call(this.param, this.md5);
}
Component.Listing.DataCache.Page.prototype.dataReceived = function (data) {
	if	(data == "") {
		this.error = true;
		this.data = new Array();
	}
	else if	(data.indexOf("KEEP#") == 0)
		this.cache.component.setMaxPage(parseInt(data.substr(5), 10) - 1);
	else {
		var	dataArr = data.split('\n');

		var	a = dataArr.shift().split('#');
		this.md5 = a.shift();
		this.cache.component.setMaxPage(parseInt(a.shift(), 10) - 1);

		this.data = new Array();
		this.error = false;

		while	(dataArr.length > 0) {
			var	e = new this.cache.dataType (this.cache, this);
			e.parse(dataArr);
			this.data.push(e);
		}
	}

	this.loading = false;
	this.onLoaded(this.param);
}


//----------------------------------------------------------------------------------------------
// LISTING COMPONENT - GENERIC DATA
//----------------------------------------------------------------------------------------------
Component.Listing.Data = function (cache, page) {
	if	(typeof cache == "undefined")
		return;
	Component.apply(this, [false]);
	this.cache = cache;
	this.page = page;
}
Component.Listing.Data.prototype = new Component;
Component.Listing.Data.prototype.parse = function (data) {
	alert("Listing using elemnts without a parse() method\n"
		+ "Skipping line: " + data.shift());
}
Component.Listing.Data.prototype.getRowStyle = function () {
	return '';
}
