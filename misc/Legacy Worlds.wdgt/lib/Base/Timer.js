Base.Timer = Base.Comp.inherits({

	/** This property contains the setTimeout() result value being used for the
	 * timer.
	 */
	timer:		null,

	/** This property contains the timestamp of the last executed tick.
	 */
	lastAt:		0,

	/** This property contains the timestamp at which the timer was paused.
	 */
	pausedAt:	0,

	/** This property is used internally to lock access to the instance's
	 * properties.
	 */
	locked:		false,

	/** This property indicates whether the timer is currently running.
	 */
	running:	false,


	/** The constructor initialises a new timer instance. It does not start
	 * the timer, however - the start() method has to be called.
	 * @param	miliseconds	the interval for the timer, in milliseconds
	 * @param	repeat		a boolean indicating whether the timer fires
	 *					only once (false) or repeatedly (true)
	 */
	constructor: function (miliseconds, repeat) {
		this.base();

		this.addSlot('start');
		this.addSlot('stop');
		this.addSlot('restart');
		this.addSlot('pause');

		this.addEvent('Tick');

		this.miliseconds	= miliseconds;
		this.repeat		= repeat;
	},

	/** This method stops the timer then destroys the instance.
	 */
	destroy: function () {
		while (this.locked) ;
		this.locked = true;

		if (this.running) {
			clearTimeout(this.timer);
			this.lastAt = this.pausedAt = 0;
			this.running = false;
		}

		this.base();
	},


	/** This method initialises the timer itself.
	 */
	start: function() {
		while (this.locked) ;
		this.locked = true;

		if (!this.running) {
			this.running = true;
			this.lastAt = new Date().getTime();
			this.setTimer();
		}

		this.locked = false;
	},

	/** This method stops the timer from running.
	 */
	stop: function() {
		while (this.locked) ;
		this.locked = true;

		if (this.running) {
			clearTimeout(this.timer);
			this.lastAt = this.pausedAt = 0;
			this.running = false;
		}

		this.locked = false;
	},

	/** This method restarts the timer. It is equivalent to calling
	 * stop() then start(), but it is faster.
	 */
	restart: function() {
		while (this.locked) ;
		this.locked = true;

		if (this.running) {
			clearTimeout(this.timer);
			this.pausedAt = 0;
		} else {
			this.running = true;
		}
		this.lastAt = new Date().getTime();
		this.setTimer();
		this.locked = false;
	},

	/** This method puts the timer on pause if it is currently running or
	 * restarts it if it had been paused earlier.
	 */
	pause: function () {
		while (this.locked) ;
		this.locked = true;

		if (this.running) {
			if (this.pausedAt == 0) {
				this.pausedAt = new Date().getTime();
				if (this.timer) {
					clearTimeout(this.timer);
				}
			} else {
				this.setTimer();
				this.pausedAt = 0;
			}
		}

		this.locked = false;
	},


	/** This method is called automatically by the setTimeout() timer; if
	 * the current instance is still supposed to be running, it will:
	 *  - re-run setTimeout() to initialise the next tick if the timer is
	 * in "repeat" mode,
	 *  - trigger the Tick event.
	 */
	tick: function() {
		while (this.locked) ;
		this.locked = true;

		var re;
		if (re = (this.running && !this.pausedAt)) {
			if (this.repeat) {
				this.lastAt = new Date().getTime();
				this.setTimer();
			} else {
				this.running = false;
				this.timer = null;
				this.lastAt = 0;
			}
		}

		this.locked = false;

		if (re) {
			this.onTick();
		}
	},

	/** This method is used internally in order to run setTimeout() with
	 * the required parameters.
	 */
	setTimer: function() {
		var _t = this.pausedAt ? (this.miliseconds - this.pausedAt + this.lastAt) : this.miliseconds;
		this.timer = setTimeout("Base.Comp.get(" + this._cid + ").tick()", _t);
	}
});
