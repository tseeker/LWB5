/** This class defines the data structure and operations required to handle a component's
 * events.
 */
Base.Comp.Evt = Base.inherits({

	/** The constructor generates an event storage for an event on a component's
	 * instance.
	 * @param	name		the name of the event
	 * @param	propagate	a boolean indicating whether this event should
	 *					be propagated to a component's children
	 */
	constructor: function (name, propagate) {
		this.id		= ++Base.Comp.Evt.lId;
		this.name	= name;
		this.propagate	= propagate;
		this.bindings	= new Base.Util.Hashtable();

		Base.Comp.Evt.list[this.id] = this;
	},

	/** This method is responsible for cleaning up an event storage instance being
	 * destroyed.
	 */
	destroy: function () {
		var k = this.bindings.keys(), i;
		for (i=0;i<k.length;i++) {
			var s = this.bindings.get(k[i]);
			s.events.remove(this.id);
			this.bindings.remove(s.id);
		}

		Base.Comp.Evt.list[this.id] = null;
		for (var i in this) {
			this[i] = null;
		}
	},

	/** This method binds the current event to a specified slot.
	 * @param	slot	the slot to which the event will be bound
	 */
	bind: function (slot) {
		if (slot && !this.bindings.containsKey(slot.id) && !slot.events.containsKey(this.id)) {
			slot.events.put(this.id, this);
			this.bindings.put(slot.id, slot);
		}
	},

	/** This method detaches the current event from a specified slot.
	 * @param	slot	the slot from which the event should be detached
	 */
	detach: function (slot) {
		if (slot && this.bindings.containsKey(slot.id) && slot.events.containsKey(this.id)) {
			slot.events.remove(this.id);
			this.bindings.remove(slot.id);
		}
	},

	/** This method triggers the execution of all the slots bound to the event.
	 */
	trigger: function() {
		var i, k = this.bindings.keys();
		for (i=0;i<k.length;i++) {
			this.bindings.get(k[i]).call(arguments);
		}
	}

}, {

	/** This class property stores the value of the last attributed event identifier.
	 */
	lId: -1,

	/** This class property stores the list of all existing events.
	 */
	list: new Array()

});
