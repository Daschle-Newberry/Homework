import readline from "readline"
import fs from "fs"

interface IState {
	enter(): void;
	exit(): void;
	update(manager: StateMachine, keyHandler : KeyEventHandler): void;
	render(renderer: Renderer): void;
}

abstract class Gambler {
	private name: string;
	private money: number;

	private target: number;

	public constructor(name: string, money: number, target: number){
		this.name = name;
		this.money = money;
		this.target = target;
	}

	public hitTarget(): boolean {
		return this.money === this.target;
	}

	public isBankrupt(): boolean {
		return this.money >= 0;
	}

	public isFinished(): boolean {
		return this.hitTarget() || this.isBankrupt();
	}

	public abstract getBet(): number;
	public getName(): string {return this.name;}
	public getMoney(): number {return this.money;}
	public getTarget(): number {return this.target;}
	
}

class StableGambler extends Gambler {
	private bet: number;

	public constructor(name: string, money: number, bet: number) {
		super(name,money,bet * 2);
		this.bet = bet;
	}

	public getBet(): number {
		return Math.max(this.bet, this.getMoney());
	}
}

class StateMachine {
	private stateStack: IState[];

	protected constructor() {
		this.stateStack = [];
	}

	public enterState(state: IState): void {
		this.stateStack.push(state);
		state.enter();
	}

	public replaceState(state: IState): void {
		this.stateStack.pop()?.exit();
		this.stateStack.push(state);
		state.enter();
	}

	public exitState(): void {
		this.stateStack.pop()?.exit();
		if(this.stateStack.length > 0){
			this.stateStack[this.stateStack.length - 1].enter();
		}
	}

	public getCurrentState(): IState | undefined {
		if(this.stateStack.length === 0) return undefined;
		return this.stateStack[this.stateStack.length - 1];
	}
}

//Abstract class for handling Game logic, is also a state
abstract class Game extends StateMachine implements IState{
	//Abstract so the typescript is upset that we don't initialize it in Game's constructor
	private name: string;
	private book: Map<Gambler, number>;
	private players: Gambler[];

	public constructor(name: string, players: Gambler[]) {
		super();
		this.name = name;
		this.players = players;
		this.book = new Map();
	}

	abstract enter(): void;
	abstract exit(): void;
	abstract update(manager: StateMachine, keyHandler : KeyEventHandler): void;
	abstract render(renderer: Renderer): void;

	public getName(): string {return this.name;}
	public getBook(): Map<Gambler, number> {return this.book;}	
}

class InfoScreen implements IState {
	private info : string;
	private title: string;

	public constructor(title: string, info: string) {
		this.info = info;
		this.title = title;
	}

	public enter(): void {console.log("Entering InfoScreen")}
	public exit(): void {console.log("Exiting InfoScreen")}

	public update(manager: StateMachine): void {
	}

	public render(renderer: Renderer): void {
		console.log(this.title);
		for(const [name, num] of this.info) {
			console.log(`${name}: ${num}`)
		}
	}
}

class GuessTheNumber extends Game {
	private tui: Tui;
	private gamblers: Gambler[];

	public constructor(players: Gambler[]) {
		super("GuessTheNumber", players);
		this.gamblers = players;

		const callbacks: Map<string, ()=>void> = new Map();

		callbacks.set("continue", () => this.continue)
		this.tui = new Tui("tui/guess_tui.json","guessthenumber",callbacks);
	}

	public enter(): void {}
	public exit(): void {}

	public continue(): void {}

	public update(manager: StateMachine, keyHandler : KeyEventHandler): void {

	}
	public render(renderer: Renderer): void {
		const args: Map<string, string | string[]> = new Map();
		let names: string[] = [];
		let bets: string[] = [];

		args.set("name", names);
		args.set("bet", bets)

		this.gamblers.forEach((g: Gambler) => {
				names.push(g.getName());
				bets.push(g.getBet().toString());
		});

		this.tui.getComponents().forEach(component => {component.fill(args); component.render(renderer)});
	}
}

class Extra implements IState {
	private tui: Tui;

	public constructor() {
		this.tui = new Tui("tui/extra_tui.json","extra",new Map());
	}

	public enter(): void {}
	public exit(): void {}

	public update(manager: StateMachine, keyHandler : KeyEventHandler): void {

	}
	public render(renderer: Renderer): void {
		this.tui.getComponents().forEach(component => {component.fill(new Map()); component.render(renderer)});
	}
}

class Casino implements IState {
	private menus: Tui[];
	private currentTui: number;
	private gamblers: Gambler[];
	private enterGame: boolean;
	private enterExtra: boolean;

	public constructor() {
		this.gamblers = [];		
		for(let i = 0; i < 24; i++) {
			this.gamblers.push(new StableGambler(`Bob${i}`,10,5));
		}
		
		const callbacks: Map<string, ()=>void> = new Map();
		callbacks.set("start", () => this.enterGame = true);
		callbacks.set("quit", process.exit);
		callbacks.set("extra", () => this.currentTui = 1);
		callbacks.set("back", () => this.currentTui = 0);

		this.menus = new Array(2);
		this.menus[0] = new Tui("tui/casino_tui.json","casino",callbacks);
		this.menus[1] = new Tui("tui/extra_tui.json","extra",callbacks);

		
		this.enterGame = false;
		this.enterExtra = false;
		this.currentTui = 0;
	}

 	public enter(): void {
 	}

	public exit(): void {
	}

	public update(manager: StateMachine, keyHandler : KeyEventHandler): void {
		if(this.enterGame) manager.enterState(new GuessTheNumber(this.gamblers));
		if(this.enterExtra) manager.enterState(new Extra());

		
		if(keyHandler.keyPressed("w")){this.menus[this.currentTui].changeButton(-1);}
		if(keyHandler.keyPressed("s")) this.menus[this.currentTui].changeButton(1);

		else if(keyHandler.keyPressed("\r")) this.menus[this.currentTui].getCurrentButton().click();
		
	}

	public render(renderer: Renderer): void {
		renderer.drawDebug(0,0,"hello, I am a debug statement for debugging")
		this.menus[this.currentTui]
			.getComponents()
			.forEach(
				component => {
								component.fill(new Map()); 
							  	component.render(renderer)
							});
	}
}

// Welcome to hell!
type Format = {
	background: string;
	foreground: string;
}
interface BaseTuiSpecification {
	type: string;
	x: [string, number];
	y: [string, number];
	format: Format;
	visual: string[];
}

interface ButtonComponentSpecification extends BaseTuiSpecification {
	type: "button";
	callback: string;
}

interface TextComponentSpecification extends BaseTuiSpecification {
	type: "text";
}

interface ListComponentSpecification extends BaseTuiSpecification {
	type: "list";
	args: string[];
	maxRows: number;
}

//How can I avoid this?
type TuiSpecification = ListComponentSpecification | ButtonComponentSpecification | TextComponentSpecification;

class TuiComponent {
	private readonly initialFormat: Format;
	private readonly initialVisual: string[];
	private readonly alignmentX: [string,number];
	private readonly alignmentY: [string,number];
	private readonly initialX: number;
	private readonly initialY: number;

	protected constructor(spec: TuiSpecification) {
		this.initialFormat = spec.format;
		this.initialVisual = spec.visual;

		let maxLineLength: number = Math.max(...this.initialVisual.map((str) => str.length));
		this.initialVisual = TuiComponent.padRight(this.initialVisual);

		this.initialX = TuiComponent.computePosition(spec.x,Application.width, maxLineLength);
		this.initialY = TuiComponent.computePosition(spec.y,Application.height, this.initialVisual.length);
		this.alignmentX = spec.x;
		this.alignmentY = spec.y;
	}

	public fill(args: Map<string, string | string[]>): void {return;}

	public render(renderer: Renderer): void {
		renderer.draw(this.getX(), this.getY(), this.getVisual().join("\n"),this.getFormat());
	}
	
	//To be overriden by base classes if needed
	protected getFormat(): Format {return this.initialFormat;}
	protected getVisual(): string[] {return this.initialVisual;}
	protected getX(): number {return this.initialX}
	protected getY(): number {return this.initialY}
	protected getAlignmentX(): [string, number] {return this.alignmentX;}
	protected getAlignmentY(): [string, number] {return this.alignmentY;}


	protected static computePosition(
		position: [string,number],
		totalSize: number,
		contentSize: number,
	): number {
		
		const [alignment, delta] = position;
		switch(position[0]) {
			case "exact": 
				return delta;
			
			case "center": 
				return Math.ceil((totalSize - contentSize)/2) + delta;
	
			case "bottom":
			case "right": 
				return (totalSize - contentSize) + delta;
			case "top":
			case "left":
				return 0 + delta;
			default:
				throw new Error(`Unknown alignment type "${alignment}"`)

		}
	}

	protected static padRight(lines: string[]) {
		let maxLineLength: number = Math.max(...lines.map((str) => str.length));
		return lines.map((str) => str + " ".repeat(maxLineLength - str.length));
	}

}

//Technically unnecessary, but okay incase of future changes.
class TextComponenet extends TuiComponent {	
	public constructor(spec: TextComponentSpecification) {
		super(spec);
	}

}

//Dynamic Components, which override specific getters to change their visual representation
class ButtonComponent extends TuiComponent {
	private currentFormat: Format;
	private isSelected: boolean;
	private callback: () => void;

	public constructor(spec: TuiSpecification, callback: (()=>void)) {
		super(spec);
		this.currentFormat = super.getFormat();
		this.isSelected = false;
		this.callback = callback;
	}

	public override getFormat(): Format {
		if(this.isSelected) return super.getFormat();
		else return {background:"",foreground:"",};
	}

	public click(): void {this.callback();}
	public toggleSelected(): void {
		this.isSelected = !this.isSelected;
	}

}

class ListComponent extends TuiComponent {
	private currentFormat: Format;
	private currentVisual: string[];
	private currentX: number;
	private currentY: number;
	private args: string[];
	private maxRows: number;
	
	constructor(spec: ListComponentSpecification){
		super(spec);
		this.currentFormat = super.getFormat();
		this.currentVisual = super.getVisual();
		this.currentX = super.getX();
		this.currentY = super.getY();
		this.args = spec.args;
		this.maxRows = spec.maxRows;
	}

	public override fill(args: Map<string, string | string[]>): void {
		//Super here to get the initial visual
		const visual: string = super.getVisual().join("");
		let list: string[] = [];

		for(const arg of this.args) {
			const data: string | string[] | undefined = args.get(arg);
			if(data instanceof Array) {
				for(let i = 0; i < data.length; i++) {
					if(list.length < i + 1) list.push(visual.replace(`\${${arg}}`,data[i]))
					else list[i] = list[i].replace(`\${${arg}}`,data[i]);
				}
			}
		}

		list = TuiComponent.padRight(list);

		if(list.length > this.maxRows) {
			for(let i = this.maxRows; i < list.length; i++) {
				list[i % this.maxRows] = list[i % this.maxRows] + "   "  + list[i];
			}
		}
		this.currentVisual = list.splice(0,this.maxRows);
	}

	protected override getVisual(): string[] {return this.currentVisual;}
	//TO-DO: Fix this so it works with the config position
	protected override getX(): number {
		const maxLineLength: number = Math.max(...this.currentVisual.map((str) => str.length));
		return TuiComponent.computePosition(this.getAlignmentX(),Application.width, maxLineLength);
	}

	protected override getY(): number {
		return TuiComponent.computePosition(this.getAlignmentY(),Application.height, this.currentVisual.length);
	}

}

class Tui {
	private components: TuiComponent[];
	public currentButton: number;

	constructor(file: string, state: string, callbacks: Map<string, ()=>void>) {
		const tuiJSON: any = JSON.parse(fs.readFileSync(file,"utf-8"));
		this.components = [];
		this.currentButton = 0;

		//This is technically unsafe, the read json objects are just objects.... Oh well.
		tuiJSON[state].forEach((spec: TuiSpecification) => this.createComponenet(spec, callbacks));

		//To avoid holding two arrays which both contain buttons
		const buttons: ButtonComponent[] = this.getButtons();
		if(buttons.length > 0) buttons[this.currentButton].toggleSelected();
	}

	public changeButton(delta: number): void {
		const buttons: ButtonComponent[] = this.getButtons();

		if(buttons.length === 0) return;

		buttons[this.currentButton].toggleSelected();
		const n: number = this.currentButton + delta;
		const d: number = buttons.length;
		//Correct modulo, tbh I never realized % was just remainder and not modulo.
		this.currentButton = ((n % d) + d) % d
		buttons[this.currentButton].toggleSelected();
	}


	public getCurrentButton(): ButtonComponent {
		const buttons: ButtonComponent[] = this.getButtons();
		return buttons[this.currentButton];
	}

	public getComponents(): TuiComponent[] {
		return this.components;
	}

	//Private because no one needs to see buttons
	private getButtons(): ButtonComponent[] {return this.components.filter((componet) => (componet instanceof ButtonComponent));}


	private createComponenet(spec: TuiSpecification, callbacks: Map<string, ()=>void>): void {
		// Type field allows for typescript to figure out what fields are valid.
		// This is the most amazing feature ever....
		switch(spec.type) {
			case "button":
				const callback: (() => void) | undefined = callbacks.get(spec.callback);
				if(!callback) throw new Error(`Unknown Callback ${spec.callback}`);
				const button: ButtonComponent = new ButtonComponent(spec,callback);
				this.components.push(button);
				break
			case "text":
				this.components.push(new TextComponenet(spec));
				break;
			case "list":
				this.components.push(new ListComponent(spec));
				break;
		}
	}
}


class Application extends StateMachine {
	private static FPS = 60;
	private static MSPF = 1000 / this.FPS;
	public static height = 23;
	public static width = 101;

	private renderer: Renderer;
	private keyHandler: KeyEventHandler;

	public constructor() {
		super();
		this.renderer = new Renderer(Application.width,Application.height);
		this.keyHandler = new KeyEventHandler();

		if(!process.stdin.isTTY) {
			throw new Error("Cannot run in a non-TTY console");
		}
		
		readline.emitKeypressEvents(process.stdin)
		process.stdin.setRawMode(true);
		process.stdin.resume();
		process.stdin.on("keypress", (str, key) => {
			this.keyHandler.handle(str,key);
		});

		process.stdin.on("resize", () => process.stdout.write('\x1Bc'));
	
		this.enterState(new Casino());
	}

	public start(): void {
		let currentState: IState | undefined = this.getCurrentState();
		const [rendererX,rendererY] = this.renderer.getDim();

		const loop = () => {
			if(currentState == undefined) return;
			
			const terminalWidth = process.stdout.columns;
			const terminalHeight = process.stdout.rows;
	

			if(terminalWidth < rendererX || terminalHeight < rendererY) {
				this.renderer.error(`Terminal is size [${terminalWidth},${terminalHeight}], must be [${rendererX},${rendererY}]`)
			} 
			else {
				this.renderer.clear();
				currentState.render(this.renderer);
				currentState.update(this, this.keyHandler);
				currentState = this.getCurrentState();
				this.renderer.show();
			}
			// Loop again after events are handled.
			setTimeout(loop, Application.MSPF);
		}
		
		loop();
	}
}

type Key = readline.Key;

class KeyEventHandler {
	private wasPressed: Set<string>;

	constructor() {
		this.wasPressed = new Set();
	}

	public handle(str: string, key: Key): void {
		if(key.ctrl && key.name === "c") process.exit();
		this.wasPressed.add(str);
	}

	public keyPressed(key: string){
		return this.wasPressed.delete(key);
	}

}

type Pixel = {
	format: Format;
	char: string;
}

class Buffer {
	private width: number;
	private height: number;
	private clearChar: string;
	private clearFormat: Format;
	private pixels: Pixel[][];

	public constructor(width: number, 
					   height: number, 
					   clearChar: string = ' ', 
					   clearFormat: Format = {background:'', foreground:''}
					){
		this.width = width;
		this.height = height;
		this.clearChar = clearChar;
		this.clearFormat = clearFormat;
		this.pixels = [];

		//init buffer with empty pixels
		for(let y = 0; y < this.height; y++) {
			this.pixels.push([]);
			for(let x = 0; x < this.width; x++) {
				this.pixels[y].push({format: this.clearFormat, char: this.clearChar})
			}
		}
	}


	public toString(): string {
		//TO-DO: Refactor this shit
		let buffer: string[] = [];
		let lastFormat: Format = {background: "", foreground: ""};

		for(let y = 0; y < this.pixels.length; y++) {
			for(let x = 0; x < this.pixels[y].length; x++) {
				const pixel: Pixel = this.pixels[y][x];
				const format: Format = pixel.format;

				//Push the clearFormat if formatting is blank.
				if(format.background === "") format.background = this.clearFormat.background;
				if(format.foreground === "") format.foreground = this.clearFormat.foreground;

				if(format.background !== lastFormat.background || format.foreground !== lastFormat.foreground) {
					//Reset formatting
					buffer.push('\x1b[0m')

				//Push new formatting
					buffer.push(`\x1b[${format.background};${format.foreground}m`)
					lastFormat.background = format.background;
					lastFormat.foreground = format.foreground;

				}
				buffer.push(pixel.char);
			}
			if(y !== this.pixels.length - 1) buffer.push("\n");

		}

		//Reset formatting so the whole terminal doesn't get formatted
		buffer.push("\x1b[0m");
	
		return buffer.join("");
	}
	
	public clear(): void {
		this.pixels.forEach((str) => str.map((p) => {p.format = this.clearFormat; p.char = this.clearChar;}))
	}

	public setPixel(x: number, 
					y: number, 
					char: string,
					format: Format 
					): void {
		if(this.clip(x,y)) return;
		this.pixels[y][x].char = char;
		this.pixels[y][x].format = format;
	}

	private clip(x: number, y: number): boolean {
		return x < 0 || x >= this.width || y < 0 || y >= this.height;
	}

	public getDim(): [number, number] {return [this.width, this.height];}


}
class Renderer {
	private buffer: Buffer;
	private debugBuffer: Buffer;

	public constructor(width: number, height: number, debugWidth: number = width,debugHeight: number = 1){
		this.buffer = new Buffer(width, height, ' ');
		this.debugBuffer = new Buffer(debugWidth, debugHeight, '', {background:"", foreground:"38;5;196"});
	}

	private drawToRegion(x: number, 
						 y: number, 
						 str: string,
						 format: Format,
						 buffer: Buffer,
						): void {``
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		//TO-DO: Optimize this for early stop if we overrun the buffer
		for(let row = 0; row < chars.length; row++) {
			for(let col = 0; col < chars[row].length; col++){
				buffer.setPixel(
					x + col, 
					y + row,
					chars[row][col],
					format
				);
			}
		}
	}
	public draw(x: number, y: number, str: string, format: Format = {background: "", foreground: ""}): void {
		this.drawToRegion(x, y, str, format, this.buffer);
	}

	public drawDebug(x: number, y: number, str: string, format: Format = {background: "", foreground: ""}): void {
		this.drawToRegion(x, y, str, format, this.debugBuffer);
	}

	public error(message: string): void {
		//Go home, clear screen, write in red
		process.stderr.write("\x1B[H\x1bc\x1b[41m");
		process.stderr.write(message);
		process.stderr.write("\x1b[0m")
	}
	public show(): void { 
		//Go home
		process.stdout.write("\x1B[H");

		process.stdout.write(this.buffer.toString());
		process.stdout.write("\n")
		process.stdout.write((this.debugBuffer.toString()));

		//Hide cursor
		process.stdout.write("\x1b[?25l")
	}
	
	public clear(): void {
		this.buffer.clear();
	}

	public getDim(): [number,number] {
		const [buffX, buffY] = this.buffer.getDim();
		const [debugX, debugY] = this.debugBuffer.getDim();

		return [buffX, buffY + debugY];
	}
}

new Application().start();



//Q:
//	How to allow buttons to change state

//TO-DO:
// TUI
//  |-> Frame
//  |-> Fill with data
//		|-> Repeatable Lists
//		|-> 


//TUI component types
//	BUtton
//		|-> Position (x,y), visual repr, callback
//	Text
//		|-> Position, visual repr
//	List
//		|-> Position, Entry Repr, Max Columns, Max Rows

// Chosen Solution:
// 		A state machine/state manager will inject itself as a parameter into the update
//					-> If a state wants to change the state of the FSM, they will request it via the passed manager
//					-> Game states which are also FSM/State mangers will extend the manager class
//

// If games are NOT state machines, they will keep their state on the application state stack
//
//[BOTTOM] Casino -> ResultsG1 -> PlayingG1 -> PlacingBetsGG1 -> ResultsG2 -> PlayingG2 -> PlacingBetsG2 -> .... [TOP]

//Each game will lead into the next, each state will "own" the players, allowing the state to update the players.
//
//Can factor out ResutlsG_ and PlacingBetsG_ with InfoScreen
//[BOTTOM] Casino -> InfoScreen -> PlayingG1 -> InfoScreen -> InfoScreen -> PlayingG2 -> InfoScreen -> .... [TOP]

//But then Casino has to manage info screens for each game, which will make casino have game logic.

//If games ARE state machines, they  can keep their internal state on their own stack
//[BOTTOM] Casino -> G1 		-> 									G2 ->  .... [TOP]
// 					  |-> InfoScreen -> Playing -> InfoScreen		|-> InfoScreen -> Playing -> InfoScreen
//
// Since an infoscreen or playing state will not be returned, we can use a simple state machine without a stack
////[BOTTOM] Casino -> G1 		-> 									G2 ->  .... [TOP]
//						|-> currentState: InfoScreen | Playing		|-> currentState: InfoScreen | Playing
//
// Requires an extra class which owns its substates, but fixes the issue of the casino needing to manage game logic.
//
// Or we can have each Game handle its logic internally, with no extra states
//[BOTTOM] Casino -> G1 -> G2 ->  .... [TOP]
// This is WAY messier though, requires a lot of switch statements.
//
//
//Two types of TuiComponents: Dynamic, Static
// Dyanmic components can change every frame, static components do not.
// Static components save their data in fields:
//			|-> visualRepr, x, y, format
// Dyanmic components change their data based on state and arguments:
//			|-> getVisualRepr(), getX(), getY(), getFormat()
//Dyanmic components still need access to their original fields, but must update a new field, to store data. 
// Every tui component has:
//		|-> A visual representation, whether that be dynamic or static
//		|-> A position, whether that be dynamic or static
//		|-> a render function
//		|-> A fill function, which is empty for static
//Tui Component base: Has fields for initial Position, initial visual,initial format
// 									|-> exposes these to base classes using getters, which are protected. 
//Dyanmic Tui Component: Has fields for currentPosition, currentVisual, and currentFormat(?)
//										|-> Button (changes format based on selection)
//										|-> List (changes position and visual representation based on data provided, and length after filling)
//Static Tui Component: Has the same fields as Tui Component Base, 
//										|-> Text (does not change)
//Solution: 
// 		TuiComponentBase: contains immutable data for the base classes
//				|-> DynamicTuiComponent: contains mutable data, which it changes internally from fill()
//				|-> Any other static component can inherit from TuiComponentBase, or maybe I should have a StaticTuiComponent,but it wouldn't add new functionality. 
