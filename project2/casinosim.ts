import readline from "readline"
import fs from "fs"

interface IState {
	enter(): void;
	exit(): void;
	update(manager: StateManager, keyHandler : KeyEventHandler): void;
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

class StateManager {
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
abstract class Game extends StateManager implements IState{
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
	abstract update(manager: StateManager, keyHandler : KeyEventHandler): void;
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

	public update(manager: StateManager): void {
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

		this.tui = new Tui("tui.json","guessthenumber",new Map());
	}

	public enter(): void {}
	public exit(): void {}

	public update(manager: StateManager, keyHandler : KeyEventHandler): void {

	}
	public render(renderer: Renderer): void {
		const args: Map<string, string | string[]> = new Map();
		let names: string[] = [];
		let bets: string[] = [];

		args.set("names", names);
		args.set("bets", bets)

		this.gamblers.forEach((g: Gambler) => {
				names.push(g.getName());
				bets.push(g.getBet().toString());
		});

		this.tui.getComponents().forEach(component => {
			renderer.draw(component.getX(),component.getY(),component.toString(args),component.getFormat());
		});	}
}

class Casino implements IState {
	private tui: Tui;
	private gamblers: Gambler[];
	private enterGame: boolean;
	private debugStatement: string = "";

	public constructor() {
		this.gamblers = [new StableGambler("Bob", 10, 5), new StableGambler("Josh", 10, 5), new StableGambler("Alice", 10, 5)];
		

		const callbacks: Map<string, ()=>void> = new Map();
		callbacks.set("start", () => this.start());
		callbacks.set("quit", process.exit);
		this.tui = new Tui("tui.json","casino",callbacks);
		
		this.enterGame = false;
	}

	public start(): void {
		this.enterGame = true;
	}
 	public enter(): void {
 	}

	public exit(): void {
	}

	public update(manager: StateManager, keyHandler : KeyEventHandler): void {
		if(this.enterGame) manager.enterState(new GuessTheNumber(this.gamblers));
		
		if(keyHandler.keyPressed("w")){this.tui.changeButton(-1);}
		if(keyHandler.keyPressed("s")) this.tui.changeButton(1);

		else if(keyHandler.keyPressed("\r")) this.tui.getCurrentButton().click();
		
	}

	public render(renderer: Renderer): void {
		this.tui.getComponents().forEach(component => {
			renderer.draw(
				component.getX(),
				component.getY(),
				component.toString(),
				component.getFormat()
			);
		});
		renderer.drawDebug(0,0,`${this.tui.currentButton}`,"");
	}
}
interface BaseTuiSpecification {
	type: string;
	x: number | string;
	y: number | string;
	formatting: string;
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
}

type TuiSpecification = ListComponentSpecification | ButtonComponentSpecification | TextComponentSpecification;

class TuiComponent {
	private x: number;
	private y: number;
	private visual: string[];
	private format: string;

	protected constructor(spec: TuiSpecification) {
		this.format = spec.formatting;
		this.visual = spec.visual;

		let maxLineLength: number = Math.max(...this.visual.map((str) => str.length));
		this.visual = this.visual.map((str) => str + " ".repeat(maxLineLength - str.length));

		this.x = TuiComponent.computePosition(spec.x,Application.width, maxLineLength);
		this.y = TuiComponent.computePosition(spec.y,Application.height, this.visual.length);

	}

	public toString(args: Map<string, string | string[]> = new Map()): string {
		return(this.visual.join("\n"));
	}

	protected getBaseFormat(): string {return this.format;}
	protected getBaseVisual(): string[] {return this.visual;}
	
	public getFormat(): string {return this.format;}
	public getX(): number {return this.x}
	public getY(): number {return this.y}

	private static computePosition(
		position: number | string,
		totalSize: number,
		contentSize: number,
	): number {
		if(typeof position === "number") return position;

		switch(position) {
			case "center":
				return Math.ceil((totalSize - contentSize)/2)
			case "bottom":
			case "right":
				return(totalSize - contentSize)
			case "top":
			case "left":
				return 0;
			default:
				throw new Error(`Unknown alignment type "${position}"`)
		}
	}

}

class TextComponenet extends TuiComponent{
	public constructor(spec: TextComponentSpecification) {
		super(spec);
	}
}

class ButtonComponenet extends TuiComponent {
	private isSelected: boolean;
	private callback: () => void;

	public constructor(spec: ButtonComponentSpecification, callback: (()=>void)) {
		super(spec);
		this.isSelected = false;
		this.callback = callback;	
	}

	public override getFormat(): string {
		if(this.isSelected) return this.getBaseFormat();
		else return "";
	}
	public click(): void {this.callback();}
	public toggleSelected(): void {
		this.isSelected = !this.isSelected;
	}

}

class ListComponent extends TuiComponent {
	constructor(spec: ListComponentSpecification){
		super(spec);
	}

	public override toString(args: Map<string, string | string[]> = new Map()): string {
		const visual: string = this.getBaseVisual().join("");
		const list: string[] = [];

		const names: string | string[] | undefined = args.get("names");
		const bets: string | string[] | undefined = args.get("bets");

		if(names instanceof Array && bets instanceof Array) {
			for(let i = 0; i < Math.min(names.length, bets.length); i++) {
				list.push(visual.replace("${name}",names[i]).replace("${bet}", bets[i]));
			}
		}

		return list.join("\n");
	}
}

class Tui {
	private components: TuiComponent[];
	public currentButton: number;

	constructor(file: string, state: string, callbacks: Map<string, ()=>void>) {
		const tuiJSON: any = JSON.parse(fs.readFileSync("tui.json","utf-8"));
		this.components = [];
		this.currentButton = 0;

		//This is technically unsafe, the read json objects are just objects.... Oh well.
		tuiJSON[state].forEach((spec: TuiSpecification) => this.createComponenet(spec, callbacks));

		//To avoid holding two arrays which both contain buttons
		const buttons = this.components.filter((componet) => (componet instanceof ButtonComponenet));
		if(buttons.length > 0) buttons[this.currentButton].toggleSelected();
	}

	public changeButton(delta: number): void {
		const buttons = this.components.filter((componet) => (componet instanceof ButtonComponenet));

		if(buttons.length === 0) return;

		buttons[this.currentButton].toggleSelected();
		const n: number = this.currentButton + delta;
		const d: number = buttons.length;
		//Correct modulo, tbh I never realized % was just remainder and not modulo.
		this.currentButton = ((n % d) + d) % d
		buttons[this.currentButton].toggleSelected();
	}


	public getCurrentButton(): ButtonComponenet {
		const buttons = this.components.filter((componet) => (componet instanceof ButtonComponenet));
		return buttons[this.currentButton];
	}

	public getComponents(): TuiComponent[] {
		return this.components;
	}


	private createComponenet(spec: TuiSpecification, callbacks: Map<string, ()=>void>): void {
		// Type field allows for typescript to figure out what fields are valid.
		// This is the most amazing feature ever....
		switch(spec.type) {
			case "button":
				const callback: (() => void) | undefined = callbacks.get(spec.callback);
				if(!callback) throw new Error(`Unknown Callback ${spec.callback}`);
				const button: ButtonComponenet = new ButtonComponenet(spec,callback);
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


class Application extends StateManager {
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
	
		this.enterState(new Casino());
	}

	public start(): void {
		let currentState: IState | undefined = this.getCurrentState();
		
		const loop = () => {
			if(currentState == undefined) return;
			
			const terminalWidth = process.stdout.columns;
			const terminalHeight = process.stdout.rows;

			this.renderer.clear();
			if(terminalWidth < Application.width || terminalHeight < Application.height) {
				console.error("Terminal too small, please resize to resume!")
			} 
			else {
			currentState.render(this.renderer);
			this.renderer.show();

			currentState.update(this, this.keyHandler);

			currentState = this.getCurrentState();
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
	format: string;
	char: string;
}

class Buffer {
	private width: number;
	private height: number;
	private pixels: Pixel[][];

	public constructor(width: number, height: number){
		this.width = width;
		this.height = height;
		this.pixels = [];
		//TO-DO: Refactor this shit
		for(let y = 0; y < this.height; y++) {
			this.pixels.push([]);
			for(let x = 0; x < this.width; x++) {
				this.pixels[y].push({format: "", char: " "})
			}
		}
	}


	public toString(): string {
		//TO-DO: Refactor this shit
		let buffer: string[] = [];
		let lastFormat: string = "";

		for(let y = 0; y < this.pixels.length; y++) {
			for(let x = 0; x < this.pixels[y].length; x++) {
				const pixel: Pixel = this.pixels[y][x];

				if(pixel.format !== lastFormat) {
					lastFormat = pixel.format;
					buffer.push("\u001b[0m");
					buffer.push(pixel.format);
				}
					buffer.push(pixel.char);
			}
			buffer.push("\n");
		}
		buffer.push("\u001b[0m");

		return buffer.join("");
	}
	
	public clear(): void {
		this.pixels.forEach((str) => str.map((p) => {p.format = ""; p.char = " ";}))
	}

	public setPixel(x: number, 
					y: number, 
					char: string,
					format: string 
					): void {
		if(this.clip(x,y)) return;
		this.pixels[y][x].char = char;
		this.pixels[y][x].format = format;
	}

	private clip(x: number, y: number): boolean {
		return x < 0 || x >= this.width || y < 0 || y >= this.height
	}

	public getDim(): [number, number] {return [this.width, this.height];}


}
class Renderer {
	private buffer: Buffer;
	private debugBuffer: Buffer;

	public constructor(width: number, height: number, debugWidth: number = width,debugHeight: number = 1){
		this.buffer = new Buffer(width, height);
		this.debugBuffer = new Buffer(debugWidth, debugHeight);
	}

	private drawToRegion(x: number, 
						 y: number, 
						 str: string,
						 format: string,
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
	public draw(x: number, y: number, str: string, format: string): void {
		this.drawToRegion(x, y, str, format, this.buffer);
	}

	public drawDebug(x: number, y: number, str: string,format: string): void {
		this.drawToRegion(x, y, str, format, this.debugBuffer);
	}

	public show(): void { 
		console.log(this.buffer.toString());
		console.log(this.debugBuffer.toString());
	}
	
	public clear(): void {
		console.clear();
		this.buffer.clear();
	}
}

new Application().start();



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
