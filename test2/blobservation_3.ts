function cwAngle(x: number, y: number) {
    const deg = 90 - Math.atan2(y,x) * (180/Math.PI);
    return (deg + 360) % 360;
}

function distance(x1: number, y1: number, x2: number, y2: number) {
    return Math.max(Math.abs(x1 - x2), Math.abs(y1 - y2));
}
class MyBlob {
    private x: number;
    private y: number;
    private size: number;

    public constructor(x: number, y: number, size: number) {
        this.x = x;
        this.y = y;
        this.size = size;
    }

    public getMove(grid: (MyBlob | undefined)[][], h: number, w: number): [number, number] {   
          if(this.size == 2 && this.x == 13) {
              console.log(`${this.x}, ${this.y}`);
            }             
        let radius: number = 0;

        let targets: MyBlob[] = [];
        while(targets.length == 0 && radius < Math.max(h,w)) {
            const inRing: MyBlob[] = this.getRing(grid, this.x, this.y, radius, h,w);
            inRing.forEach((blob: MyBlob) => {if(this.isValidTarget(blob)) targets.push(blob)});
            radius++;
        }

        if(targets.length <= 0) return [0,0];
        
        targets.sort((a: MyBlob, b: MyBlob) => {
            const aDist: number = distance(a.getX(), a.getY(), this.x, this.y);
            const bDist: number = distance(b.getX(), b.getY(), this.x, this.y);

            if(aDist !== bDist) {
                return bDist - aDist;
            }
            else if (a.getSize() != b.getSize()) {
                return b.getSize() - a.getSize();
            }
            else {
                return cwAngle(b.getX() - this.x, b.getY() - this.y) - cwAngle(a.getX() - this.x, a.getY() - this.y);
            }
        });

        const target: MyBlob = targets[0];

        const dx: number = target.getX() - this.x;
        const dy: number = target.getY() - this.y;
        const stepX = dx == 0 ? 0 : dx/Math.abs(dx);
        const stepY = dy == 0 ? 0 : dy/Math.abs(dy);

        return [stepX, stepY];
    }

    private getRing(grid: (MyBlob | undefined)[][], x: number, y: number, radius: number, h: number, w: number): MyBlob[] {
        const res: MyBlob[] = [];

        for(let dy = -radius; y <= radius; y++) {
            for(let dx = -radius; x <= radius; x++) {
                const xRing = x + dx;
                const yRing = y + dy;

                if(xRing < 0 || yRing < 0 || xRing >= w || yRing >= h) continue;

                if(Math.max(Math.abs(dx)), Math.max(Math.abs(dy)) !== radius) continue;

                if(grid[yRing][xRing] !== undefined) res.push(grid[yRing][xRing])
            }
        }

        return res;
    }
    private isValidTarget(blob: MyBlob | undefined): boolean {
        return blob !== undefined && blob.getSize() < this.size;
    }
    public getX(): number { return this.x; }
    public getY(): number { return this.y; }

    public getSize(): number { return this.size; }
    public addSize(delta: number) { this.size += delta; }

    public setX(x: number): void { this.x =x; }
    public setY(y: number): void { this.y =y; }


}


class Blobservation {
    private h: number;
    private w: number;
    private blobs: Set<MyBlob>;
    private grid: (MyBlob | undefined)[][];
    public constructor(w: number, h: number = w) {
        console.log(`hw: ${w},${h}`)
        this.w = w;
        this.h = h;
      
        
        this.blobs = new Set();
        this.grid = [];
        for(let y = 0; y < this.h; y++) {
            this.grid.push(new Array());
            for(let x = 0; x < this.w; x++) {
                this.grid[y].push(undefined);
            }   
        }
    }

    public populate(blobs: {x: number, y: number, size: number}[]): void {
        for(const blob of blobs) {
            if(typeof blob.x !== 'number' || typeof blob.y !== 'number' || typeof blob.size !== 'number') throw new Error("Invalid blob argument");
            console.log(`${blob.x}, ${blob.y}, ${blob.size}`);

            const current: MyBlob | undefined = this.grid[blob.y][blob.x];
            if(current == undefined) {
                const blobObj: MyBlob = new MyBlob(blob.x, blob.y,blob.size);
                this.grid[blob.y][blob.x] = blobObj;
                this.blobs.add(blobObj);
            } else {
                current.addSize(blob.size);
            }
        }
    }

    public move(iters: number = 1): void {
        if(typeof iters !== "number") throw new Error("Invalid argument type for move");
        if(iters <= 0) throw new Error(`Invalid amount of iterations ${iters}`);

        console.log(`Num of iters: ${iters}`)
        while(iters > 0) {
            const moves: Map<MyBlob, {x: number, y: number}> = new Map();

            const minSize: number = Math.min(...Array.from(this.blobs).map(b => b.getSize()));
            for(const blob of this.blobs) {
                if(blob.getSize() <= minSize) { moves.set(blob, {x:blob.getX(), y:blob.getY()}); continue };
                const [stepX,stepY] = blob.getMove(this.grid, this.h, this.w);
                const x = stepX + blob.getX();
                const y = stepY + blob.getY();
                moves.set(blob, {x:x, y:y});
            }

            this.resetGrid();
            for(const [blob,move] of moves) {
                blob.setX(move.x);
                blob.setY(move.y);
                this.mergeCells(blob,blob.getX(),blob.getY());
            }
            iters--;
        }
    }

    public print_state(): number[][] {
        const sorted: MyBlob[] = [...this.blobs].sort((a: MyBlob,b: MyBlob) => {
            if(a.getX() !== b.getX()) {
                return a.getX() - b.getX();
            }
            return a.getY() - b.getY();
        });
        
        return sorted.map((blob: MyBlob) => [blob.getX(),blob.getY(),blob.getSize()]);;
    }

    public printGrid(): void {
        for(let y = 0; y < this.h; y++) {
            for(let x = 0; x < this.w; x++) {
                const cell: MyBlob | undefined = this.grid[y][x];
                const pos: String = cell == undefined ? "-" : cell.getSize().toString();

                process.stdout.write(`${pos} `);
            }
            console.log();
        }
        console.log();
    }

    private resetGrid(): void {
        this.grid = [];
        for(let y = 0; y < this.h; y++) {
            this.grid.push(new Array());
            for(let x = 0; x < this.w; x++) {
                this.grid[y].push(undefined);
            }   
        }
    }
    private mergeCells(blob: MyBlob, x: number, y: number) {
        const cell: MyBlob | undefined = this.grid[y][x]
        if(cell === undefined) this.grid[y][x] = blob;
        else {
            //Doesn't matter which blob gets deleted
            this.blobs.delete(blob);
            cell.addSize(blob.getSize());
        }
    }
}


// const generation0 = [
// 		{x:0,y:4,size:3},
// 		{x:0,y:7,size:5},
// 		{x:2,y:0,size:2},
// 		{x:3,y:7,size:2},
// 		{x:4,y:3,size:4},
// 		{x:5,y:6,size:2},
// 		{x:6,y:7,size:1},
// 		{x:7,y:0,size:3},
// 		{x:7,y:2,size:1}];
// 	let blobs = new Blobservation(8);
// 	blobs.populate(generation0);
//     blobs.printGrid();
// 	blobs.move();
//     blobs.printGrid();


// const gen = [
// {x:0, y:3, size:7},
// {x:1, y:6, size:1},
// {x:1, y:18,size:3},
// {x:2, y:11,size:3},
// {x:3, y:3, size:3},
// {x:4, y:12,size:1},
// {x:4, y:14,size:1},
// {x:5, y:0, size:6},
// {x:5, y:7, size:5},
// {x:5, y:18,size:8},
// {x:6, y:13,size:3},
// {x:8, y:4, size:4},
// {x:8, y:7, size:6},
// {x:8, y:10,size:2},
// {x:8, y:13,size:7},
// {x:8, y:6, size:5},
// {x:10,y: 2,size:3},
// {x:10,y: 7,size:5},
// {x:11,y: 11,size:2},
// {x:12,y: 6, size:1},
// {x:12,y: 14,size:2},
// {x:13,y: 1, size:2},
// {x:14,y: 9, size:5},
// {x:16,y: 0, size:1},
// {x:16,y: 11,size:3},
// {x:16,y: 16,size:3},
// {x:17,y: 6, size:1},
// {x:18,y: 10,size:5},
// {x:18,y: 13,size:7},
// {x:18,y: 18,size:5},
// {x:19,y: 2, size:6},
// {x:19,y: 16,size:8}
// ];
// const blobs = new Blobservation(20,20);
// 	blobs.populate(gen);
//     blobs.printGrid();
// 	blobs.move();
//     blobs.printGrid();

//[[0, 6, 5], [1, 5, 3], [3, 1, 2], [4, 7, 2], [5, 2, 4], [6, 7, 3], [7, 1, 3], [7, 2, 1]]
//[[1, 5, 3], [1, 7, 5], [3, 1, 2], [4, 7, 2], [5, 2, 4], [6, 7, 3], [7, 1, 3], [7, 2, 1]]