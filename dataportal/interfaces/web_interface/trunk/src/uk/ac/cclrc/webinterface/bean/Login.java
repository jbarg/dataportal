package uk.ac.cclrc.webinterface.bean ;

//stuff needed for jsf app
import javax.faces.application.Application;
import javax.faces.component.html.HtmlOutputText;
import javax.faces.component.html.HtmlPanelGrid;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;

//normal j2se stuff
import java.util.List;

public class HelloBean {
    private int numControls;
    private HtmlPanelGrid controlPanel;
    public int getNumControls() {
        return numControls;
    }
    public void setNumControls(int numControls) {
        this.numControls = numControls;
    }
    
    public HtmlPanelGrid getControlPanel() {
        return controlPanel;
    }
    public void setControlPanel(HtmlPanelGrid controlPanel) {
        this.controlPanel = controlPanel;
    }
    public void addControls(ActionEvent actionEvent) {
        Application application =
                FacesContext.getCurrentInstance().getApplication();
        List children = controlPanel.getChildren();
        children.clear();
        for (int count = 0; count < numControls; count++) {
            HtmlOutputText output = (HtmlOutputText)application.
                    createComponent(HtmlOutputText.COMPONENT_TYPE);
            output.setValue(" " + count + " ");
            output.setStyle("color: blue");
            children.add(output);
        }
    }
    public String goodbye() {
        return "success";
    }
}



