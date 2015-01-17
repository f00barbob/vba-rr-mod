require "helpers"

check_interval = 240
ROM_name_addr = 0x80000A0
emu_name = getascii16(memory.readbyterange(ROM_name_addr,16))
print("emu_name = " .. emu_name
autosave_prefix = "E:\\" .. emu_name

function do_autosave()
	print("checking for autosave...")
	print(prefix .. emu_name)
	if os.date("%M") == "00" then
		print ("time was! actually writing:")
		hax.save(prefix .. "3242.sav")
	end

end

while true do
	if (vba.framecount() % check_interval) == 0 then
		do_autosave()
	end

	emu.frameadvance()
end
