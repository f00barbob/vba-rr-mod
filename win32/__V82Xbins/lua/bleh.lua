

print(hax.get_pid())
print(hax.get_rom_name())
print(hax.read_pokestring_delimited(0xD009,0xA))
for i=0, hax.get_cheat_count() -1 do
	print("code: " .. hax.get_cheat_code(i) .. " desc: " .. hax.get_cheat_desc(i))
end
