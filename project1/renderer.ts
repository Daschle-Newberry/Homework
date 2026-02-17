export class Renderer{
	private width: number;
	private height: number;
	private buffer: string[][];

	constructor(width: number, height: number){
		this.width = width;
		this.height = height;
		this.buffer = [];
		for(let y = 0; y < this.height;  y++){
			this.buffer.push([]);
			for(let x = 0; x < this.width; x++){
				this.buffer[y].push("");
			}
		}
	}

	public draw(x: number, y: number, str: string): void {
		const chars: string[][] = str.split("\n").map((s) => s.split(""));
		
		for(let row = 0; row < chars.length && y + row < this.height; row++){
			if(y + row > this.height) break;
			for(let col = 0; col < chars[row].length && x + col < this.width; col++){
				if(x + col > this.width) break;
				this.buffer[y + row][x + col] = chars[row][col];
			}
		}
	}

	public show(): void { 
		const str: string = this.buffer.map((s) => s.join("")).join("\n");
		console.log(str);
	}
	public clearBuffer(): void {
		this.buffer = [];
		for(let y = 0; y < this.height;  y++){
			this.buffer.push([]);
			for(let x = 0; x < this.width; x++){
				this.buffer[y].push("");
			}
		}
	}

	public clearScreen(): void {
		console.clear();
	}
}



