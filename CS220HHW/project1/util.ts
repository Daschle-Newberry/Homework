export function concatByLine(strs: string[]): string {
	const strsLines: string[][] = strs.map((str) => str.split("\n"));
	let res: string = "";
	for(let j = 0; j < strsLines[0].length; j++) {
		for (const lines of strsLines) {
			res += lines[j];
		}
		res += "\n";
	}
	return res;
}
