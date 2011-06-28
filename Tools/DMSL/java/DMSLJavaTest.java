import DMSLEngine.*;
public class DMSLJavaTest {
	static { System.loadLibrary("dmsl"); }

	public static void main(String argv[]) throws java.io.IOException
	{
	  	String configurationFile = "../conf.ini";
		String inputFile = "../testfiles/test1.txt";
		String outputFile = "../testfiles/output.txt";
		String paramFile = "../testfiles/params.txt";
		String component = "loadconfirmation_alternative";

		DecisionMaker.Initialize(configurationFile);
	
		do {
			DecisionMaker d = new DecisionMaker();
			if(!d.Compile(inputFile)) {
				System.out.println(d.GetError());
				break;
			}
			System.out.println("Compilation done...");

			if(!d.WriteText(outputFile)) {
				System.out.println("Could not dump rules to file " + outputFile);
				break;
			}
			System.out.println("Written rules to file...");

			if(!d.LoadProfile(paramFile)) {
				System.out.println(d.GetError());
				break;
			}
			System.out.println("Profile loading done...");

			d.SetAttribute("context.installation", "kiosk");
			d.SetAttribute("user.\"web expertise\"", "expert");
			d.SetAttribute("user.\"visual ability\"", "sighted");

			if(!d.Evaluate(component)) {
				System.out.println(d.GetError());
				break;
			}
			System.out.println("Evaluation done...");

			StringVector v;
			System.out.println("Stereotypes:");
			v = d.GetStereotypes();
			for(int i = 0; i < v.size(); ++i)
		  		System.out.println("\t" + v.get(i));

			System.out.println("Components:");
			v = d.GetComponents();
			for(int i = 0; i < v.size(); ++i)
		  		System.out.println("\t" + v.get(i));
				
			System.out.println("Decisions:");
			v = d.GetDecisions();
			for(int i = 0; i < v.size(); ++i)
		  		System.out.println("\t" + v.get(i));
			
			System.out.println("Total decisions: " + v.size());
			System.out.println("Exiting...");
		} while(false);

		DecisionMaker.Cleanup();
	}
}
