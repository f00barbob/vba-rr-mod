function palettefart()

	for i = 0, 0x10, 2 do
		memory.writebyte(math.random(0x6000000,0x6017FFF), math.random(0,65))

	end

end

vba.registerafter(palettefart)
vba.registerbefore(palettefart)
gui.register(palettefart)
while true do 


	emu.frameadvance()

end
