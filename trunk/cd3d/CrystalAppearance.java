/* This class extends the %%\textsf{Appearance}%% class provided
 * by Java 3D to add some features specific to %%\cd%% such
 * as the menu items to change appearance attributes. */
import javax.media.j3d.*;
import javax.vecmath.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class CrystalAppearance extends Appearance{
  public CrystalAppearanceOptions cap;
  private String name;
  CrystalOptionDialog cod;

  public static Color C3f2C( Color3f C3f ){
    Color c;
    if ( C3f != null ){
      c = new Color( C3f.x, C3f.y, C3f.z );
    } else {
      c = Color.black;
      /* This allows for the possiblity that Java3D
       * returns a null Color3f when not yet set.*/
    }
    return c;
  }

  public static Color3f C2C3f( Color c ){
    return new Color3f( (float)( c.getRed()/255 ),
			(float)( c.getGreen()/255 ),
			(float)( c.getBlue()/255 ) );
  }

  public CrystalAppearance( String name, CrystalAppearanceOptions cap ){
    this.cap = cap;
    this.name = name;

    setColoringAttributes( cap.coloring );
    setLineAttributes( cap.lines );
    setMaterial( cap.material );
    setPointAttributes( cap.points );
    setPolygonAttributes( cap.polygons );
    setTransparencyAttributes( cap.transparency );
  }

  public void setCapabilityBits(){
    // These are the capability bits for this, i.e. the appearance object, not the constituent
    // objects e.g. ColoringAttributes etc...
    if ( cap.allowsColoringAttributesChange ){
      setCapability( Appearance.ALLOW_COLORING_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_COLORING_ATTRIBUTES_READ );
    }
    if ( cap.allowsMaterialChange ){
      setCapability( Appearance.ALLOW_MATERIAL_WRITE );
      setCapability( Appearance.ALLOW_MATERIAL_READ );
    }
    if ( cap.allowsTransparencyAttributesChange ){
      setCapability( Appearance.ALLOW_TRANSPARENCY_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_TRANSPARENCY_ATTRIBUTES_READ );
    }
    if ( cap.allowsPointAttributesChange ){
      setCapability( Appearance.ALLOW_POINT_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_POINT_ATTRIBUTES_READ );
    }
    if ( cap.allowsLineAttributesChange ){
      setCapability( Appearance.ALLOW_LINE_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_LINE_ATTRIBUTES_READ );
    }
    if ( cap.allowsPolygonAttributesChange ){
      setCapability( Appearance.ALLOW_POLYGON_ATTRIBUTES_WRITE );
      setCapability( Appearance.ALLOW_POLYGON_ATTRIBUTES_READ );
    }
  }

  /* Provide a menu of choices for this appearance. */
  public MenuItem getAppearanceChoices(){
    MenuItem mi = new MenuItem( "Appearance" );
    cod = getOptionDialog();
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	cod.show();
      }
    });
    return mi;
  } 

  public CrystalOptionDialog getOptionDialog(){
    cod = new CrystalOptionDialog( name, cap );
    if ( cap.allowsColorChange ){
      cod.addJPanel( new SimpleColourPanel( cap ), "Colour" );
    }
    if ( cap.allowsMaterialChange ){
      cod.addJPanel( new ComplexColourPanel( cap ), "Material" );
    }
    if ( cap.allowsTransparencyAttributesChange ){
      cod.addJPanel( new TransparencyPanel( cap ), "Transparency" );
    }
    if ( cap.allowsPointAttributesChange ){
      // At least this will be set for any geometry changes.
      cod.addJPanel( new GeometryPanel( cap ), "Geometry" );
    }
    return cod;
  }

  public CrystalOptionDialog getOptionDialog( MenuItem mi ){
    cod = getOptionDialog();
    mi.addActionListener( new ActionListener(){
      public void actionPerformed( ActionEvent e ){
	cod.show();
      }
    });
    return cod;
  }
}













