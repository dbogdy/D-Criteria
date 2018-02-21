#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <fstream>

#include <algorithm>

// Data Struct for load. Type: 0 - meteor shower, 1 - asteroids
typedef struct{
  std::string name;
  double a, e, i, q, peri, node;
  int type;
} object;

// The list for loaded objects
typedef std::vector<object> object_list;

// Data Struct for store
typedef struct
{
	object obj1;
	object obj2;
    double dsh;
	double dacs;
	double dj;
} associated;

// List for results data
typedef std::vector<associated> associated_list;

// Transform from deg in rad
const double deg2rad = 3.141592653589793238463/180.0;

// Load data function. Load from files
bool loaddata_shower(object_list *list);
bool loaddata_asteroids(object_list *list);

// Data functions for D_acs, D,sh and D_h
double d_asher(object obj1, object obj2, float prag);
double d_sh(object obj1, object obj2, float prag);
double d_jopek(object obj1, object obj2, float prag);

int main()
{
	object_list all_obj; // Variable for loaded data
	associated_list fin; // Variable for results

	// Load data from file
	if ( !(loaddata_shower(&all_obj)) )    { std::cout << "shower file not found" << std::endl; return -1; }
    if ( !(loaddata_asteroids(&all_obj)) ) { std::cout << "asteroids file not found" << std::endl; return -1; }

 
	
	// Compute and results
	for (object_list::iterator it = all_obj.begin(); it != all_obj.end(); /* ++it*/) 
	{
		for (object_list::iterator jt = all_obj.begin(); jt != all_obj.end(); ++jt)
		{
			associated step;
			if (it->type == jt->type) continue;
			
			step.obj1 = *it;
			step.obj2 = *jt;
			
			// D-criteria metrics
			step.dacs = d_asher(*it, *jt, 0.06);
			step.dsh =  d_sh   (*it, *jt, 0.21);
			step.dj =   d_jopek(*it, *jt, 0.19);
			
			fin.push_back(step);
		}

			// Delete the computed object
			it = all_obj.erase(it);
	} 
	
	// Open file
	std::ofstream file;
	file.open ("test1.txt", std::ofstream::out | std::ofstream::app);

	
	//Store data to file
	for( associated n : fin) 
	{
		// If the metrics are larger then threshold
		if(n.dacs == -999 && n.dsh == -999 && n.dj == -999) continue;
		
		file << n.obj2.name << "," << n.obj2.a << "," << n.obj2.e << "," << n.obj2.i << "," << n.obj2.q << "," << n.obj2.peri << "," << n.obj2.node << ","
			 << n.obj1.name << "," << n.obj1.a << "," << n.obj1.e << "," << n.obj1.i << "," << n.obj1.q << "," << n.obj1.peri << "," << n.obj1.node << ","
			 << n.dacs << "," << n.dsh << "," << n.dj << "\n";
	}
	file.close();
	
	return 0;
}

// Function: load showers
bool loaddata_shower(object_list *list)
{
	// Variable used for store data from file
	char u_f, temp[256], name_t[50], add[4];
    object pos = { .name = "", .a= 0.0, .e= 0.0, .i= 0.0, .q = 0.0, .peri= 0.0, .node= 0.0, .type= 0 };

	// The number of lines and columns from file
    int col = 0, line = 0;

	//Open the file
  	FILE *file = fopen("data/stream_showers1.dat", "r");
  	if (file == NULL) 
  	{
		std::cout << "Can't open file stream_showers.dat\n"; return false;
  	}

	//Store data from file
    while (fgets(temp, sizeof(temp), file) != NULL)
    {
        col = sscanf(temp, "%s %s %lf  %lf  %lf  %lf  %lf  %lf",add, name_t, &pos.a, &pos.q, &pos.e, &pos.peri, &pos.node, &pos.i);
        if(col < 8)
        { 
            std::cout << "In file stream_showers.dat" << " at line " << line << " ar " << col << "/8 cols ... skip\n";
        }
        else
        {
            pos.type = 0;
            pos.name = name_t;
			list->push_back(pos);

            line++;
        }
    }
    std::cout << "In file stream_showers.dat has " << line << " lines\n";
    fclose(file);

    return true;
}

// Function: load asteroids
bool loaddata_asteroids(object_list *list)
{
	// Variable used for store data from file
	char u_f, temp[256], name_f[50];
    object pos = { .name = "", .a= 0.0, .e= 0.0, .i= 0.0, .q = 0.0, .peri= 0.0, .node= 0.0, .type= 0 };

	// The number of lines and columns from file
	int col = 0, line = 0;

	//Open the file
	FILE *file = fopen("data/mpcorb-light.dat", "r");
	if (file == NULL) 
	{
		std::cout << "Can't open file mpcorb-light.dat\n"; return false;
	}

	//Store data from file
    while (fgets(temp, sizeof(temp), file) != NULL)
    {
        col = sscanf(temp, "%s %lf  %lf  %lf  %lf  %lf", name_f, &pos.a, &pos.e, &pos.i, &pos.peri, &pos.node);
        if(col < 6)
        { 
            //std::cout << "In file mpcorb-light.dat" << " at line " << line << " ar " << col << "/6 cols ... skip\n";
        }
        else
        {
            pos.q = pos.a * (1 - pos.e); 
            pos.type = 1; pos.name = name_f;
            list->push_back(pos);

            line++;
        }
    }
    std::cout << "In file mpcorb-light.dat has " << line << " lines\n";
    fclose(file);

    return true;
}

// D-acs function
double d_asher(object obj1, object obj2, float prag)
{
	double total= 0.0, elem1 = 0.0, elem2 = 0.0, elem3 = 0.0;

	elem1 = (obj1.a - obj2.a)/3.0;
	elem2 =  obj1.e - obj2.e; 
	elem3 = 2.0*sin( (obj1.i - obj2.i)*deg2rad*0.5 );

	total = sqrt(elem1*elem1+elem2*elem2+elem3*elem3);
	
	if(total > prag) {return -999;}
	return total; 
}

double eq_I12(object obj1, object obj2)
{
	return acos(cos( obj1.i*deg2rad ) * cos( obj2.i*deg2rad ) +
				sin( obj1.i*deg2rad ) * sin( obj2.i*deg2rad ) *
				cos((obj1.node - obj2.node)*deg2rad) );
}

double eq_t12 (object obj1, object obj2, double I12 , double sign = 1) {
    
    return (obj1.peri - obj2.peri)*deg2rad + 
            sign * 2 * asin( cos( (obj1.i + obj2.i )/2 * deg2rad )* sin( (obj1.node - obj2.node)/2 * deg2rad ) * (1/cos( I12/2 ) ) );
}

bool obj_compare_2node (object obj1, object obj2, double I12) {
    if (I12 < 0) {std::cout << "I12 < 0 !!!!" ; return false;}
    if ( ( obj1.node - obj2.node > 180 ) && ( cos(I12/2) > 0.0 ) ) {return true;}
    return false;
}

// D-sh Function
double d_sh(object obj1, object obj2, float prag)
{
	double total= 0.0, elem1 = 0.0, elem2 = 0.0, elem3 = 0.0, elem4 = 0.0, elem5 = 0.0, I12 = 0.0, t12 = 0.0;

	elem1 = obj1.e - obj2.e;
	elem2 = obj1.q - obj2.q;

	I12 = eq_I12(obj1, obj2);
	elem3 = 2 * sin( I12/2 );

	elem4 = ( obj1.e + obj2.e ) / 2;
	
	if ( obj_compare_2node (obj1, obj2, I12 ) ) 
	{
		t12 = eq_t12 (obj1, obj2, I12,-1.);
    }
	else 
	{
		t12 = eq_t12 (obj1, obj2, I12);
    }
	elem5 = 2*sin(t12/2);
	
	total = sqrt(elem1*elem1 + elem2*elem2 + elem3*elem3 + (elem4*elem4)*(elem5*elem5));
	
	if(total > prag) {return -999;}
	return total;
}

// D-h function
double d_jopek(object obj1, object obj2, float prag)
{
	double total = 0.0, elem1 = 0.0, elem2 = 0.0, elem3 = 0.0, elem4 = 0.0, elem5 = 0.0, I12 = 0.0, t12 = 0.0;

	elem1 =  obj1.e - obj2.e;
	elem2 = (obj1.q - obj2.q)/(obj1.q + obj2.q);

	I12 = eq_I12(obj1, obj2);
	elem3 = 2*sin(I12/2);
	elem4 = (obj1.e + obj2.e)/2;
	
	if ( obj_compare_2node (obj1, obj2, I12 ) ) 
	{
		t12 = eq_t12 (obj1, obj2, I12,-1.);
    }
	else 
	{
		t12 = eq_t12 (obj1, obj2, I12);
    }
	elem5 = 2*sin(t12/2);
	
	total = sqrt(elem1*elem1+elem2*elem2+elem3*elem3+(elem4*elem4)*(elem5*elem5));
	
	if(total > prag) {return -999;}
	return total;
}