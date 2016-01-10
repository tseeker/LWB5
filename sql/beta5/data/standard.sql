-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/data/readonly.sql
--
-- Beta 5 games:
-- Read-only data insertion
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


-- ECM probability table
COPY ecm FROM STDIN;
0	1	0
0	2	0
0	3	0
0	4	1
0	5	99
1	1	0
1	2	0
1	3	10
1	4	70
1	5	20
2	1	0
2	2	10
2	3	70
2	4	15
2	5	5
3	1	5
3	2	70
3	3	15
3	4	8
3	5	2
4	1	60
4	2	30
4	3	6
4	4	4
4	5	0
\.

-- ECCM probability table
COPY eccm FROM STDIN;
0	0	99
0	1	1
0	2	0
0	3	0
0	4	0
1	0	20
1	1	70
1	2	10
1	3	0
1	4	0
2	0	10
2	1	20
2	2	60
2	3	10
2	4	0
3	0	5
3	1	10
3	2	20
3	3	60
3	4	5
4	0	0
4	1	5
4	2	10
4	3	40
4	4	45
\.


-- Default rules
COPY rule_def FROM stdin;
military_level	0
pop_growth_factor	1
if_productivity	30
if_productivity_factor	10
base_income	2
factory_cost	10
turret_cost	20
mf_productivity	10
mf_productivity_factor	10
workunits_turret	1500
workunits_gaship	4500
workunits_fighter	2000
workunits_cruiser	20000
workunits_bcruiser	22000
build_cost_turret	400
build_cost_gaship	750
build_cost_fighter	500
build_cost_cruiser	5000
build_cost_bcruiser	15000
if_cost	400
mf_cost	400
battle_losses	100
unhappiness_factor	100
effective_fleet_power	100
planet_max_pop	10000
capital_ship_speed	1
prevent_hs_exit	0
planet_destruction	0
turret_power	10
gaship_power	5
fighter_power	10
cruiser_power	40
bcruiser_power	80
gaship_upkeep	40
fighter_upkeep	50
cruiser_upkeep	500
bcruiser_upkeep	1500
research_percent	200
gaship_space	3
fighter_space	1
cruiser_haul	20
bcruiser_haul	15
ecm_level	0
eccm_level	0
gaship_pop	200
hs_beacon_level	0
probe_tech	0
\.


-- Rule handlers
COPY rule_handler FROM STDIN;
unhappiness_factor	UpdateHappiness
planet_max_pop	UpdateMaxPopulation
effective_fleet_power	UpdateFleetPower
gaship_power	UpdateFleetPower
fighter_power	UpdateFleetPower
cruiser_power	UpdateFleetPower
bcruiser_power	UpdateFleetPower
ecm_level	UpdateCommTech
eccm_level	UpdateCommTech
\.


-- Research definitions
COPY research FROM STDIN;
1	7000	50000	0	1	FALSE
2	5000	15000	0	0	FALSE
3	4000	20000	0	2	FALSE
4	6000	30000	0	2	FALSE
5	13333	5000	0	1	TRUE
6	29000	300000	0	1	FALSE
7	44667	50000	0	2	TRUE
8	10333	40000	0	2	FALSE
9	10333	40000	0	2	FALSE
10	18000	50000	0	2	FALSE
11	18000	50000	0	0	FALSE
12	38000	80000	2	2	FALSE
13	57667	15000	1	2	TRUE
14	54000	100000	1	0	FALSE
15	37554	100000	1	2	FALSE
16	14667	100000	0	0	FALSE
17	34889	100000	1	0	FALSE
18	21777	150000	1	0	FALSE
20	99628	500000	1	2	FALSE
21	211873	1000000	2	2	FALSE
22	99628	500000	1	0	FALSE
23	51519	30000	1	0	TRUE
24	28777	100000	1	2	FALSE
25	87925	500000	1	0	FALSE
26	23777	100000	2	1	FALSE
27	135777	500000	1	0	FALSE
28	241036	800000	1	0	FALSE
29	112000	200000	1	2	FALSE
30	68000	1500000	2	2	FALSE
31	44000	400000	1	2	FALSE
32	68667	60000	1	2	TRUE
33	300000	400000	2	2	FALSE
34	174333	80000	1	2	TRUE
35	127036	800000	2	2	FALSE
36	72146	600000	1	2	FALSE
37	68369	300000	1	2	FALSE
38	98072	500000	1	1	FALSE
39	212466	700000	2	1	FALSE
40	172837	1000000	1	0	FALSE
41	280449	2000000	1	0	FALSE
42	554218	1500000	1	2	FALSE
43	860476	100000	1	0	TRUE
44	49036	500000	2	1	FALSE
45	49036	500000	1	1	FALSE
46	112048	1000000	2	1	FALSE
48	238218	1500000	1	1	FALSE
49	238218	1500000	1	1	FALSE
50	160195	800000	1	2	FALSE
51	302260	1500000	2	1	FALSE
52	875013	3000000	2	1	FALSE
53	342195	1500000	1	0	FALSE
54	839548	2000000	1	1	FALSE
55	517576	2000000	1	2	FALSE
56	740101	70000	1	2	TRUE
57	263996	1500000	1	0	FALSE
58	241036	1500000	1	0	FALSE
59	403005	120000	1	2	TRUE
60	307222	3000000	2	1	FALSE
61	136195	500000	1	1	FALSE
62	1106581	2500000	1	2	FALSE
63	583758	2500000	1	1	FALSE
64	628073	5000000	1	1	FALSE
65	628073	5000000	1	1	FALSE
68	319418	2000000	1	1	FALSE
69	1019937	150000	1	1	TRUE
70	1949373	5000000	1	0	FALSE
71	630811	4000000	2	1	FALSE
72	529228	2500000	2	0	FALSE
73	965308	3800000	1	2	FALSE
74	1295217	4000000	1	2	FALSE
75	2537057	7000000	1	2	FALSE
76	3725240	200000	1	2	TRUE
77	2228337	10000000	1	1	FALSE
78	1251897	4000000	1	2	FALSE
79	845313	4000000	1	0	FALSE
80	2719164	10000000	1	1	FALSE
81	2709164	10000000	1	2	FALSE
82	3616595	12000000	1	1	FALSE
83	4321117	10000000	1	2	FALSE
84	805637	5000000	1	1	FALSE
85	2088549	8000000	1	1	FALSE
86	4642983	12000000	1	1	FALSE
87	3642219	170000	1	2	TRUE
88	7457771	15000000	1	1	FALSE
89	4000692	10000000	1	1	FALSE
90	2627540	10000000	1	1	FALSE
\.


-- Research dependencies
COPY research_dep FROM STDIN;
5	1
6	1
6	2
16	2
46	2
8	3
9	3
17	3
10	4
11	4
38	4
7	6
51	6
63	6
71	6
15	8
18	8
36	8
15	9
24	9
26	9
27	9
14	10
30	10
50	10
12	11
30	11
31	11
13	12
27	14
29	14
20	15
22	15
38	15
17	16
20	16
22	16
25	16
23	17
43	17
21	18
44	18
45	18
68	46
21	20
42	20
48	20
76	21
40	22
49	22
57	22
25	24
35	24
36	24
37	24
72	25
39	26
28	27
53	27
58	27
42	28
55	28
33	29
34	29
33	30
32	31
35	31
52	33
73	35
50	36
55	36
61	36
53	37
57	37
63	37
39	38
71	38
54	39
41	40
64	40
65	40
63	41
70	41
73	41
79	41
43	42
62	42
74	42
49	44
59	44
46	45
48	45
59	48
62	48
65	48
64	49
51	50
52	51
54	53
74	53
78	53
56	55
75	55
72	57
73	57
71	58
77	58
79	58
68	61
69	61
85	61
70	62
69	63
89	63
90	63
86	64
82	65
80	70
81	70
82	70
89	71
78	72
84	72
83	73
75	74
77	74
83	74
85	74
76	75
90	78
83	79
89	79
86	80
88	80
87	81
88	81
89	84
60	46
60	22
\.


-- Research effects
COPY research_effect FROM STDIN;
1	military_level	1
3	battle_losses	-2
4	if_cost	80
4	if_productivity_factor	2
5	if_productivity_factor	-2
5	mf_productivity_factor	3
5	unhappiness_factor	5
6	military_level	1
7	effective_fleet_power	-5
7	unhappiness_factor	-4
8	if_cost	40
8	if_productivity_factor	1
8	mf_cost	40
8	mf_productivity_factor	1
9	if_cost	40
9	if_productivity_factor	1
9	mf_cost	40
9	mf_productivity_factor	1
10	pop_growth_factor	1
12	unhappiness_factor	-1
13	if_productivity_factor	-2
13	mf_productivity_factor	-3
13	unhappiness_factor	-5
15	research_percent	10
17	research_percent	10
18	unhappiness_factor	-2
20	research_percent	10
21	base_income	2
23	base_income	-1
23	if_productivity_factor	-2
23	research_percent	25
24	battle_losses	-5
24	if_cost	80
24	if_productivity_factor	2
24	mf_cost	80
24	mf_productivity_factor	2
26	gaship_pop	75
29	if_cost	120
29	if_productivity_factor	3
30	pop_growth_factor	1
31	if_productivity_factor	-1
31	mf_productivity_factor	-1
31	unhappiness_factor	-2
32	if_productivity_factor	-1
32	mf_productivity_factor	-1
32	unhappiness_factor	-4
33	pop_growth_factor	1
34	pop_growth_factor	1
34	unhappiness_factor	5
35	planet_max_pop	10000
36	if_cost	120
36	if_productivity_factor	3
36	mf_cost	120
36	mf_productivity_factor	3
37	battle_losses	-5
37	if_cost	80
37	if_productivity_factor	2
37	mf_cost	80
37	mf_productivity_factor	2
38	effective_fleet_power	10
39	gaship_pop	75
41	capital_ship_speed	1
42	research_percent	10
43	if_productivity_factor	-3
43	mf_productivity_factor	-3
43	research_percent	50
44	ecm_level	1
45	eccm_level	1
46	hs_beacon_level	1
48	eccm_level	1
49	ecm_level	1
51	battle_losses	-2
52	battle_losses	-2
53	battle_losses	-2
53	if_cost	80
53	if_productivity_factor	2
53	mf_cost	80
53	mf_productivity_factor	2
54	gaship_pop	75
55	if_cost	80
55	if_productivity_factor	2
55	mf_cost	80
55	mf_productivity_factor	2
55	unhappiness_factor	7
56	if_productivity_factor	-2
56	mf_productivity_factor	-2
56	unhappiness_factor	-7
57	battle_losses	-4
59	eccm_level	-1
59	ecm_level	-1
59	unhappiness_factor	-4
60	hs_beacon_level	1
61	turret_power	3
62	research_percent	10
63	military_level	1
64	ecm_level	1
65	eccm_level	1
68	turret_power	3
69	if_productivity_factor	-4
69	mf_productivity_factor	6
69	unhappiness_factor	10
70	capital_ship_speed	1
71	fighter_power	4
71	gaship_power	2
73	planet_max_pop	10000
74	battle_losses	-2
74	if_cost	80
74	if_productivity_factor	2
74	mf_cost	80
74	mf_productivity_factor	2
75	if_cost	160
75	if_productivity_factor	4
75	mf_cost	120
75	mf_productivity_factor	3
76	base_income	3
76	if_productivity_factor	10
76	mf_productivity_factor	-5
76	research_percent	-50
76	unhappiness_factor	15
77	battle_losses	-5
78	if_cost	80
78	if_productivity_factor	2
78	mf_cost	80
78	mf_productivity_factor	2
80	prevent_hs_exit	1
81	if_productivity_factor	-3
81	unhappiness_factor	-5
82	eccm_level	1
83	planet_max_pop	10000
84	capital_ship_speed	1
84	effective_fleet_power	5
85	turret_power	3
86	ecm_level	1
87	if_cost	120
87	if_productivity_factor	3
87	unhappiness_factor	7
88	planet_destruction	1
89	bcruiser_power	20
89	cruiser_power	10
90	effective_fleet_power	10
\.


-- Research descriptions
COPY research_txt FROM STDIN;
1	en	Fighters	Sir! We have successfully researched a new type of ship, the Fighter! These ships are faster and more efficient at combating enemy ships than our current Ground Assault ships.
2	en	Hyperspace Basics	Sir! Our scientists have made major progress in understanding the basics behind Hyperspace theory. Those basic hyperspace capabilities are the very beginning of a new area in space flight and allow a wide range of new experiments.
3	en	Advanced Materials	Sir! Our scientists just discovered new production methods that will allow us to create hardened materials! Our factories must be upgraded in order to start producing those new alloys.
4	en	Bio-engineering	Sir! Our scientists worked hard to improve our empire's knowledge of bio-engineering, which will greatly improve the production of basic goods, as well as pave the way for further advances.
5	en	Martial law	Sir! We can enact Martial Law to force our people to work in the military's best interest! They probably won''t be too happy about it, and our economy might suffer, but who cares? Our military production will be greatly improved!
6	en	Cruisers	Sir! We have successfully researched a new type of ship, the Cruiser! These are able to travel outside of our solar system thanks to the recent developments in Hyperspace technology, and can carry our current ships in their holds.
7	en	Civilian Transportation Act	This law grants our citizens the right to use military ships to move between planets. This increases happiness but reduces battle efficiency of our ships since they have civilians on board.
8	en	Room Temperature Superconductors	Sir! Our scientists have worked on a new type of electronic circuitry that will allow us to greatly improve the efficiency of our factories, using those room temperature superconductors.
9	en	Nanotechnologies	Sir! Our scientists have made major progress in miniaturisation. With this technology our researchers have gained the capability to work at the "nano" level.
10	en	Advanced Hospitals	Sir! We have successfully researched a new public service! We hope that these "Advanced Hospitals" will improve the health of our subjects so that they are less likely to bite the dust at an unprofitable moment.
11	en	High-efficiency Hydroponics	Recent progress in bio-engineering has allowed for new farming techniques, that are safer for the planet and allow for further studies in green technologies.
12	en	Safe Recreational Drugs	Recent progress in the field of farming has allowed our scientists to develop safe recreational drugs such as the so-called "Space weed". This will improve commerce and make the population happier, since these drugs can be used instead of traditional anaesthetics in our hospitals.
13	en	Legalize Space Weed	Yeah, maaan ... I mean, Sir ... Enacting this law will allow every citizen in our empire to smoke Space Weed as they see fit, without any harmful effect for their health. I mean... huh, What was I saying again? ... Ah, yes! Well, they might be a bit inefficient, because they'll still be stoned, but they''ll be happy!
14	en	Cloning Techniques	Sir! Their stem cells analysis have finally brought our scientists a new breakthrough. Our researchers have succeeded in cloning various lifeforms. 
15	en	Nano-scale Computers	Sir, their recent progresses in nanotechnologies has allowed our scientists to create a new generation of computers. This new miniaturised computer allows us to improve research efficiency and opens a brand new field of research.
16	en	Quantum Gravitation	These advances in the field of quantum theory will open a new area for further studies.
17	en	Miniaturised Particle Colliders	These new and small particle colliders increase research in many areas, increasing lab outputs.
18	en	Advanced Communications	Having achieved major breakthroughs in electronic research, our scientists are now able to apply this research to the communications field, where some interesting developments are expected. 
20	en	Quantum Computers	Based on the new discoveries in the quantum theory field, these new computers are much more efficient and faster. Equipping our labs will be expensive but the advantages of the increased research speed should be incredible.
21	en	Economy Globalisation	Recent advances in both communications and computer capacities have allowed us to set up an empire wide economic system that should increase our planets base income.
22	en	Hyperspace Theory	Our scientists have developed and tested a complete advanced theory regarding the structure of Hyperspace! We need to upgrade their labs for them to continue researches in this field.
23	en	Increased Research Grants	This law allows to divert a higher percentage of income towards research thus allowing faster discoveries but reducing income.
24	en	Hardened Alloys	These new improved alloys are more resistant and will allow for less losses in battle as well as future advances in industry research.
25	en	Experimental Anti-Matter Production	Recent advances in new alloy production and a better knowledge of quantum theory have allowed our scientists to produce anti-matter for the first time. Research should be continued in this area since the applications could be tremendous.
26	en	Nanofiber Armor	Sir! Applying nanotechnologies to the military field our scientists have managed to produce nano-fiber armors for our ground troups. Since our soldiers will be better protected against rioters we'll need to send less of them on the ground to take control of a planet.
27	en	Lifeform Engineering	Our scientists have devised a method to design lifeforms from scratch! Although this breakthrough has no direct application, further research should be funded, the potential gains are tremendous!
28	en	Sentient Lifeform Engineering	Sir! Our biologists have finally managed to create intelligent lifeforms from scratch, thus opening a brand new field of studies.
29	en	Cloning Vats	Our scientists have perfected their cloning techniques, allowing them to grow real clones in vats. Our industrial production could be greatly improved using this technology!
30	en	Nourishment Purification	Sir! We have successfully researched a new way to improve food, Nourishment purification! This process will give our subjects a healthier lifespan by removing all the nasties in their food.
31	en	Green Production	These new production methods are safer for planetary ecology, making the population happier but slightly reducing factory productivity.
32	en	Biosphere Protection Pact	Enforcing this law forces the industrial sector to use greener production methods that make the population happier but reduce productivity.
33	en	Corpse Reanimation	Sir! Our scientists have established a new technology, Corpse Reanimation! This technology will mean about 60% of our subjects will be able to be brought back to life after death through the use of modified cloning vats. This should lead to a decrease in total death and an increase in workers for our factories.
34	en	Forced Human Cloning	Sir! We could enact a law that would allow our government to clone citizens and boost our population growth! Our people wouldn't be too happy about it though...
35	en	Arcologies	Sir! We are now able to build arcologies, which will allow us to house loads more citizens on our empire's planets!
36	en	Robotics	Recent advances in electronics and new materials have allowed our engineers to design autonomous robots that will help our workers and improve the production of our factories.
37	en	Adaptive Materials	New developments in research have allowed our scientists to create materials that adapt to the needs of our civilians, thus providing even more resistent alloys.
38	en	Cybernetic Interfaces	Sir! Our scientists have found a way to interface the human brain with a machine. This new technology allows a direct interface between electronic and biological systems, making our ships more reactive to their pilots' commands.
39	en	Exoskeleton	Sir! Our scientists have managed to improve even further the equipment of our ground troups. With those new exoskeletons we will require to send even less GA ships to get the inhabitants of another planet to share our views..
40	en	Temporal Mechanics	The progress of our scientists has enabled us to better understand temporal phenomenons, which should allow for a wide range of new discoveries. Further funding of this area of study is a necessity.
41	en	Space-time folding	Sir! Our scientists have pushed the limit of Hyperspace theory! This will allow us to design better, faster ship engines and further advances are to be expected! However, upgrading our fleets as well as our laboratories might be a bit expensive...
42	en	Biological Computers	Our scientists have found a way to integrate intelligent lifeforms into our computers, thus improving calculation capabilities. Upgrading our computers, though expensive, should allow our current research projects to reach completion faster.
43	en	Science Golden Age	Enacting this law permits us to divert more resources towards research at the expense of other necessities.
44	en	Wide Band Jamming	This basic jamming technology allows us to try and prevent the planets we attack from transmitting data to their allies. Therefore defensive procedures can potentially be disrupted. 
45	en	Fast Burst Transmission	This new technology renders our stellar and interstellar communications less prone to jamming and interception.
46	en	Hyperspace Beacon	Placed in hyperspace around our planets this beacon provides an "anchor" for your ships and those of your alliance, thus reducing the losses in fleet stationed in hyperspace.
48	en	Quantum Encryption	Quantum computers have allowed tremendous progress in encryption algorithms. We can now be more efficient in preventing the enemy from disrupting our communications.
49	en	Entropy Generator	By applying hyperspace theory to telecommunications we've found a new way to disrupt enemy communications, making it even harder for them to transmit accurate data to their allies.
50	en	Surgical Robots	This technology provides new advances in the medical field, allowing for further researches in this area. Further funding should bring quite interesting breakthroughs.
51	en	Medical Bays	Our engineers have modified the designs for our capital ships in order to include highly advanced medical bays, in which the pilots can be healed when they are wounded in combat. This improvement will reduce our losses, but our fleets and factories must be upgraded first.
52	en	Resurrection tanks	Our engineers have updated the designs for our capital ships. The medical bays will now integrate the equipment required to raise our pilots from the dead, further reducing battle losses.
53	en	Self-healing Materials	Sir! Our scientists have have found a way to grow advanced, self-healing materials. This new technology will allow us to provide regeneration and auto-repair capabilities to both our ships and factories. We will thus greatly reduce our losses in battle and gain productivity in our industrial sector.
54	en	Self-repairing Exoskeleton	Sir! Incorporating the newest alloys to our soldiers exoskeletons, our scientists have set up self-repairing exoskeletons. Even more efficient against attacking crowds, this new armor generation will reduce even more the number of GA ships required to take control of a foreign planet.
55	en	Biological Drones	This technology allows industry to use specifically designed lifeforms to replace workers in the factories. These lifeforms will be much more efficient than human beings, but our citizens might not like getting sacked in favour of their new replacements.
56	en	Ban Biological Drones	To fight the decrease in happiness caused by biological drones, we can enact a law that bans their presence in our empire. This law counters all effects of biological drones and restores factories to their previous level of productivity. 
57	en	Force fields	Our scientists have found a way to create force fields. The direct military application is the addition of shields to our current fleets, which will reduce losses.
58	en	Lifeform Energy Manipulation	Sir! Our scientists have managed to engineer lifeforms capabable of manipulating energy. This new discovery lets us foresee some astonishing future breakthroughs.
59	en	Civilian Communication Act	Passing this law will permit civilians access to our militaries' advanced communication networks. Civilians will be happier since they can keep in touch with friends, but this civilian use of military installations could disrupt anti-jamming and jamming systems!
60	en	Hyperspace Probing Beacon	Adding probing systems to hyperspace beacons, this technology allows for early detection of enemy ships stationed in hyperspace around our planets.
61	en	Automated Turrets	Sir! Including robotics in our turrets will improve their accuracy and firing efficiency. With these new turrets we will be able to better defend our planets.
62	en	Interstellar University	Sir! Thanks to our recent improvements in communications and computing capacities we've brought education to a new scale. Our interstellar universities will allow us to better adapt education programs to students'' needs and to improve cooperation between our research labs.
63	en	Battle Cruisers	Sir! We have successfully researched a new type of ship, the Battle Cruiser! These ships are an improvement on our Cruisers as they are faster and more deadly. However, they are considerably more expensive to build and carry less of our system ships.
64	en	Phase Neutraliser	Applying temporal mechanics to communications, our scientists have managed to find better ways to disrupt enemy data flows, thus reducing their chances of accurate data being sent to their allies.
65	en	Multiphasic Transmission	Applying temporal mechanics to communication, our scientists have managed to use it to transmit data in fluctuating phases. This further protects our communications from enemy jamming technologies.
68	en	Sensor Turrets	Sire dude! Equipping our turrets with sensors designed out of our probe technology, we can gain in the accuracy of our aiming, thus becoming more efficient.
69	en	Global Defense Bill	This new law goes even further than the Martial Law to cope with the military's will. But our citizens will even less appreciate it.
70	en	Wormhole Theory	Going even further than space-time folding, our scientists have written a theory that, when put into practice, would allow us to manipulate wormholes. Of course further studies are required to reach any real application.
71	en	Biological Propulsion Systems	Sir! Our scientists have found a way to create and grow artificial lifeforms capable of basic space flight. These can be used to replace our most simple ships in order to gain efficiency.
72	en	Mass Anti-matter Production	Using newly acquired technologies our scientists have acheived mass production of anti-matter thus opening a brand new field of applications.
73	en	Singularity Housing	Using the principles of Space-time folding, our engineers have improved our arcology design. Hyperspace and force-field generators must be integrated into our existing arcologies in order to allow our planets to house even more citizens.
74	en	Intelligent Materials	Bringing sentience to the materials they use, our scientists provide us with wonderful new ways of building stuff.
75	en	Automated Factories	Sir! Using sentient materials in our factories will allow us to gain production efficiency.
76	en	Wild Capitalism	This law allows the industrial sector to use any means necessary to increase profit. As a consequence, base planetary income and industrial factory benefits are increased but military factories, research and happiness suffer from it.
77	en	Adaptive Plating	Sir! Our scientists have come up with a new way to reduce battle damage. They have integrated intelligent materials in our ships' plating thus offering them better defensive abilities.
78	en	Anti-matter Generators	Sir! Our scientists have designed a new way to produce energy. Equipping our factories with these anti-matter generators will improve our productivity.
79	en	Biological Subspace Control	Sir! Our scientists have engineered some new astonishing lifeforms. These artificial creatures are capable of controlling subspace fields. This major discovery opens a new era for our future studies.
80	en	Wormhole Collapsing	Sir, our scientists have devised a new defence. This technology allows our planets to build counter-measures that will allow them to prevent unwanted hyperspace windows from forming in orbit. This should prevent 10% of an enemy fleet from exiting hyperspace above the planet and delay them in hyperspace for 1 more hour.
81	en	Wormholes	This technology applies space-time folding principles to spacebats, allowing your citizens to move freely between the planets in your empire. They will now be living in ecstacy as they will be able to visit their families, friends and dolphins.
82	en	Subspace Data conduit	Applying wormhole theory to communications has allowed our scientists to create subspace conduits to transmit data, thus hiding it even better from enemy disruption techniques.
83	en	Self-sustained Arcologies	Sir! By combining their expertise in hyperspace theory and biological engineering, our scientists have found a way to create "grown" housing that will provide room and nourishment for our citizens. This will allow us to sustain more people on our planets.
84	en	Matter Anti-matter Engines	Sir! Our scientists have found a new application for matter anti-matter reactions: propulsion systems! This new line of engines should increase our ships efficiency greatly.
85	en	Biological Turrets	Building our turrets with intelligent materials should allow us to take advantage of their sentience to gain in accuracy and fire power.
86	en	Localised Wormhole Destabilisation	Applying wormhole collapsing technologies in a localised way allows us to disrupt subspace data conduits and other long range communication, rending them less efficient.
87	en	Wormhole Lockdown	This law prevents your citizens from using the planetary gateways, cancelling the effects of the wormhole technology.
88	en	Wormhole Super Nova	Sir! With this technology we can initiate a chain reaction on a planetary wormhole that will cause it to flare up and destroy anything in the vicinity, including the planet and scaring the living hell out of the planet's neighbours.
89	en	Biological Hyperspace Engines	Sir! Our scientists have found a way to grow and nurture organic hyperspace engines. We will now be able to grow living capital ships, improving our fleets' efficiency.
90	en	Matter Anti-matter Missiles	Using matter / anti-matter reactions in our warheads should greatly increase the damage caused by our ships.
\.



-- Peacekeepers AI
INSERT INTO player (userid, hidden) VALUES (
	(SELECT id FROM main.account WHERE name = 'AI>Peacekeepers'), 't'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'military_level', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'pop_growth_factor', '1'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'if_productivity', '30'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'if_productivity_factor', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'base_income', '2'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'factory_cost', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'turret_cost', '20'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'mf_productivity', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'mf_productivity_factor', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'workunits_turret', '1500'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'workunits_gaship', '4500'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'workunits_fighter', '2000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'workunits_cruiser', '20000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'workunits_bcruiser', '22000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'build_cost_turret', '400'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'build_cost_gaship', '750'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'build_cost_fighter', '500'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'build_cost_cruiser', '5000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'build_cost_bcruiser', '15000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'if_cost', '400'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'mf_cost', '400'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'battle_losses', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'unhappiness_factor', '100'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'effective_fleet_power', '200'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'planet_max_pop', '10000'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'capital_ship_speed', '3'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'prevent_hs_exit', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'planet_destruction', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'turret_power', '10'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'gaship_power', '5'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'fighter_power', '30'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'cruiser_power', '120'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'bcruiser_power', '240'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'gaship_upkeep', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'fighter_upkeep', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'cruiser_upkeep', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'bcruiser_upkeep', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'research_percent', '200'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'gaship_space', '1'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'fighter_space', '1'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'cruiser_haul', '50'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'bcruiser_haul', '50'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'ecm_level', '4'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'eccm_level', '4'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'gaship_pop', '200'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'hs_beacon_level', '0'
);
INSERT INTO rule (player, name, value) VALUES (
	(SELECT id FROM player WHERE userid = (SELECT id FROM main.account WHERE name = 'AI>Peacekeepers')),
	'probe_tech', '0'
);
