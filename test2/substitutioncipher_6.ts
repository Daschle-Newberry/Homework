//https://www.codewars.com/kata/52eb114b2d55f0e69800078d/train/javascript kyu 6
// Used compiled javascript to submit

class SubstitutionCipher {
    private input: string;
    private output: string;

    public constructor(input: string, output: string) {
        this.input = input;
        this.output = output;
    }

    public encode(message: string): string {
        const positions: Map<string, number> = new Map();

        this.input.split("").forEach((c: string, i: number) => { positions.set(c,i) });

        let res = "";
        message.split("").forEach((c: string) => {
            const pos: number | undefined = positions.get(c);
            
            if(pos === undefined) res += c;
            else res += this.output.charAt(pos);
        })
        return res;
    }

     public decode(cipher: string): string {
        const positions: Map<string, number> = new Map();

        this.output.split("").forEach((c: string, i: number) => { positions.set(c,i) });

        let res = "";
        cipher.split("").forEach((c: string) => {
            const pos: number | undefined = positions.get(c);
            
            if(pos === undefined) res += c;
            else res += this.input.charAt(pos);
        })
        return res;
    }
}