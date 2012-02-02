/**
 *	LineCounter component
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	August 2007
 */

using std;
using #sparrowlib;
spw  = sparrowlib::sparrow();

const classId = "LineCounter";

//-----------------------------------------------------------------------

function extract_source(uri)
{
	local index = strsub(uri, "/");
	while (index != -1)
	{
		uri = strslice(uri, index +1, 0);
		index = strsub(uri, "/");
	}

	return uri;
}

//-----------------------------------------------------------------------
function CountFileLOCs(display)
{
	return [
		{.display : display },
		{ .total_lines : 0 },
		{ "()" : (
			method (script) {
				if (typeof(script) != "Table" and typeof(script) != "Object")
					return;
				script = spw.decorate(script);	///< installs dynamic dispatching to handle

				local uri = script.GetURI();
				local lines=0;

				fp = fileopen(uri, "rt");
				while (not fileend(fp)) {
					line = filegetline(fp);
					++lines;
				}
				fileclose(fp);

				self[uri] = lines;
				self.total_lines += lines;

				spw::print(self.display + ": ", extract_source(uri), " -- LOCs: ", lines, "\n");
			})
		}
	];
}

//-----------------------------------------------------------------------

function CountLOCs() {
	function for_each(t, f) { foreach(local x, t) f(x); return f; }

	spw::print("\n--- Counting lines in workspace: \n\n");
	
	local scripts = spw.components.ProjectManager.GetResources("Script");
	local scriptLines = for_each(scripts, CountFileLOCs("script")).total_lines;
	spw::print("\n--- total lines in scripts: ", scriptLines, "\n");

	local textFiles = spw.components.ProjectManager.GetResources("TextFile");
	local textFileLines = for_each(textFiles, CountFileLOCs("text file")).total_lines;
	spw::print("\n--- total lines in text files: ", textFileLines, "\n");

	local genericFiles = spw.components.ProjectManager.GetResources("GenericFile");
	local genericFileLines = for_each(genericFiles, CountFileLOCs("generic file")).total_lines;
	spw::print("\n--- total lines in generic files: ", genericFileLines, "\n");
	
	local totalLines = scriptLines + textFileLines + genericFileLines;
	spw::print("\n--- total lines in workspace: ", totalLines, "\n");

	return totalLines;
}

//-------------------------------------------------------//
//---- Component Registration ---------------------------//

onevent ClassLoad
{
	spw::registercomponent(classId);
	spw::setcomponentmetadata(
		classId, "Workspace Line Counter",
		"A utility for counting lines of all scripts in opened workspace",
		"Themistoklis Bourdenas <themis@ics.forth.gr>",
		"alpha"
	);

	spw::registerimage("linecount", spw::installationdir() + "/resources/linecount.png");

	spw::class_decl_required_member_function(classId, "CountLines", "uint (void)",
		"Return the total line of codes in the workspace");
	
	spw::class_decl_required_member_command(
		[
			{.class			: "UserCommandDesc"	},
			{.class_id		: classId			},
			{.function_name	: "CountLinesCmd"	},
			{.flags			: 7					},
			{.image			: "linecount"		}
		],
		"/{200}Tools/{100}Count Lines",
		"Display the total line of codes in the workspace"
	);
}

//-----------------------------------------------------------------------

onevent ClassUnload {}

//-------------------------------------------------------//
//---- Instance Creation --------------------------------//

onevent Constructor
{
	spw::inst_impl_required_member_function(classId, "CountLines", CountLOCs);
	spw::inst_impl_required_member_command(classId, "CountLinesCmd", CountLOCs);
}

//-----------------------------------------------------------------------

onevent Destructor
{
}

//-----------------------------------------------------------------------