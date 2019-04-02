const fs = require('fs')
const filename = process.argv.slice(2)[0]
console.log(filename)
function hexdump(filename) {
  let buffer = fs.readFileSync(filename)
  console.log(buffer)
  console.log(buffer.length)
  let lines = []

  for (let i = 0; i < buffer.length; i += 16) {
    let address = `00000000${i.toString(16)}` // address
    let block = buffer.slice(i, i + 16) // cut buffer into blocks of 16
    let hexArray = []
    let asciiArray = []
    let padding = ''

    for (let value of block) {
      hexArray.push(`00${value.toString(16)}`)
      asciiArray.push(value >= 0x20 && value < 0x7f ? String.fromCharCode(value) : '.')
    }

    // if block is less than 16 bytes, calculate remaining space
    if (hexArray.length < 16) {
      let space = 16 - hexArray.length
      padding = ' '.repeat(space * 2 + space + (hexArray.length < 9 ? 1 : 0)) // calculate extra space if 8 or less
    }

    let hexString =
      hexArray.length > 8
        ? hexArray.slice(0, 8).join(' ') + '  ' + hexArray.slice(8).join(' ')
        : hexArray.join(' ')

    let asciiString = asciiArray.join('')
    let line = `${address}  ${hexString}  ${padding}|${asciiString}|`
    // let line = `${hexString}  ${padding}|${asciiString}|`
    lines.push(line)
  }

  return lines.join('\n')
}

console.log(hexdump(filename))